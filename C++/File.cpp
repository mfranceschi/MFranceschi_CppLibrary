//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes
#pragma warning( disable: 26444) // Warning that occurs when using imbue.

#include <algorithm>
#include <locale>
#include <codecvt>
#include "File.h"
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#include "Toolbox.h"
#include <Windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

static const size_t NBR_BITS_TO_READ_ENCODING = 3;

namespace File
{
/////////////////////////////////////////////////////////////////  PRIVATE

//------------------------------------------------------------------ Types

	static class codecvt_utf8_pers : public std::codecvt<char, wchar_t, std::mbstate_t>
	{
	public:
		virtual ~codecvt_utf8_pers() noexcept {}
	};
//-------------------------------------------------------------- Constants

	const static codecvt_utf8_pers CC_UTF8;
	// const static std::codecvt_utf8<char> CC_UTF8;
	const static std::codecvt_utf16
		<char, 0x10ffff, std::little_endian> CC_UTF16;
	const static locale locUTF8(locale(), &CC_UTF8);
	const static locale locUTF16LE(locale(), &CC_UTF16); 

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

	/* FUNCTION DECLARATIONS */
#ifdef _WIN32
	static bool ExistsFromCharArrayWindows(const char* filename);
	static filesize_t SizeFromCharArrayWindows(const char* filename);
	#ifdef UNICODE // LPCWSTR
	static bool ExistsFromWchar_tArrayWindows(const wchar_t* filename);
	static filesize_t SizeFromWchar_tArrayWindows(const wchar_t* filename);
	#endif
#else // POSIX
	static bool ExistsFromCharArrayPOSIX(const char* filename);
	static filesize_t SizeFromCharArrayPOSIX(const char* filename);
#endif

	/* FUNCTION DEFINITIONS */

#ifdef _WIN32
	static bool ExistsFromCharArrayWindows(const char* filename)
	{
#ifdef UNICODE // LPCWSTR
		wchar_t* converted = Toolbox::ToWchar_t(filename);
		auto result = ExistsFromWchar_tArrayWindows(converted);
		delete[] converted;
		return result;
#else // LPCSTR
		DWORD attr = GetFileAttributes(filename);
		return attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY);
#endif
	}

	static filesize_t SizeFromCharArrayWindows(const char* filename)
	{
#ifdef UNICODE // LPCWSTR
		wchar_t* converted = Toolbox::ToWchar_t(filename);
		auto result = Size(converted);
		delete[] converted;
		return result;
#else // LPCSTR
		HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (file == INVALID_HANDLE_VALUE) return -1;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		CloseHandle(file);
		return filesize_t(res.QuadPart);
#endif
	}

#ifdef UNICODE
	static filesize_t SizeFromWchar_tArrayWindows(const wchar_t* filename)
	{
		HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (file == INVALID_HANDLE_VALUE) return -1;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		CloseHandle(file);
		return filesize_t(res.QuadPart);
	}

	static bool ExistsFromWchar_tArrayWindows(const wchar_t* filename)
	{
		DWORD attr = GetFileAttributes(filename);
		return attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY);
	}
#endif

#else // POSIX
	static bool ExistsFromCharArrayPOSIX(const char* filename)
	{
		struct stat t;
		return !stat(filename, &t);
	}

	static filesize_t SizeFromCharArrayPOSIX(const char* filename)
	{
		struct stat t;
		if (!stat(filename, &t)) return -1;
		return filesize_t(t.st_size);
	}
#endif

	/* LOW-LEVEL FILE HANDLING */
#ifdef _WIN32 // Win32
	#define _OPEN_FILE_(filename, fd) _sopen_s(&fd, filename, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD)
#else // POSIX
	#define _OPEN_FILE_(filename, fd) ((fd = open(filename, O_RDONLY)) == -1)
	#define _read read /* POSIX form */
	#define _close close /* POSIX form */
#endif

	

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	bool Exists(const char* filename)
	{
#ifdef _WIN32 // Win32
		return ExistsFromCharArrayWindows(filename);
#else // POSIX
		return ExistsFromCharArrayPOSIX(filename);
#endif
	}
	
#if defined _WIN32 && defined UNICODE
	bool Exists(const wchar_t* filename)
	// Algorithm: https://stackoverflow.com/questions/3828835.
	{
		return ExistsFromWchar_tArrayWindows(filename);
	}
#endif

	bool IsEmpty(const char* filename, size_t charsToRead)
	{
		int file;
		bool forReturn;

		if (_OPEN_FILE_(filename, file))
			forReturn = true;
		else
		{
			char* content = new char[charsToRead];
			forReturn = !(_read(file, content, charsToRead) != charsToRead);
			delete[] content;
			_close(file);
		}

		return forReturn;
	}

	void Open(ifstream& ifs, const char* filename,
		encoding_t encoding)
	{
		ifs.close();
		if (encoding == ENC_UNKNOWN)
			encoding = File::Encoding(filename);

		if (encoding == ENC_UTF8)
		{
			ifs.open(filename);
			ifs.imbue(locUTF8);
			ifs.seekg(3);
		}
		else if (encoding == ENC_UTF16LE)
		{
			ifs.open(filename, ios_base::binary);
			ifs.imbue(locUTF16LE);
			ifs.seekg(2, ios_base::beg);
		}
		else if (encoding == ENC_DEFAULT)
		{
			ifs.open(filename, ios_base::binary);
		}
	}

	filesize_t Size(const char* filename)
	{
#ifdef _WIN32 // Win32
		return SizeFromCharArrayWindows(filename);
#else // POSIX
		return SizeFromCharArrayPOSIX(filename);
#endif
	}

#if defined _WIN32 && defined UNICODE
	filesize_t Size(const wchar_t* filename)
	{
		HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (file == INVALID_HANDLE_VALUE) return -1;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		CloseHandle(file);
		return filesize_t(res.QuadPart);
	}
#endif

	encoding_t Encoding(const char* filename)
	{
		int file;
		encoding_t forReturn;

		if (_OPEN_FILE_(filename, file))
			forReturn = ENC_UNKNOWN;
		else
		{
			char bits[NBR_BITS_TO_READ_ENCODING];
			int ret_read = _read(file, bits, NBR_BITS_TO_READ_ENCODING);
			
			if (ret_read != NBR_BITS_TO_READ_ENCODING)
				forReturn = ENC_UNKNOWN;

			if (bits[0] == 0xff && bits[1] == 0xfe)
				forReturn = ENC_UTF16LE;
			else if (ret_read == 3 && bits[0] == 0xef && bits[1] == 0xbb && bits[2] == 0xbf)
				forReturn = ENC_UTF8;
			else
				forReturn = ENC_DEFAULT;

			_close(file);
		}
		return forReturn;
	}

	std::ostream& operator<< (std::ostream& os, const encoding_t& enc)
	{
		switch (enc)
		{
		case ENC_UTF16LE:
			os << "UTF-16LE";
			break;
		case ENC_UTF8:
			os << "UTF-8";
			break;
		case ENC_UNKNOWN:
			os << "<encoding-error>";
			break;
		case ENC_DEFAULT:
			os << "<encoding-unknown>";
			break;
		}
		return os;
	}
} 
