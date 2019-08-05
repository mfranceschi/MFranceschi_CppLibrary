//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes
#pragma warning( disable: 26444) // Warning that occurs when using imbue.

#include <algorithm>
#include <codecvt>
#include "File.h"
#include <fcntl.h>
#include <locale>
#include <map>

#ifdef _WIN32
#include <io.h>
#include "Toolbox.h"
#include <Windows.h>
#else
#include <sys/mman.h>
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

	// Data structure used to store informations about files opened with Open.
	struct ReadFileData {
		const char* memptr; // Holds the file data.

#ifdef _WIN32
		HANDLE fileHandle; // File HANDLE
		HANDLE mappingHandle; // File Mapping HANDLE
#else
		int fd; // File descriptor
		filesize_t size; // Size of the file = size of the mmap allocation.
#endif
	};

//-------------------------------------------------------------- Constants

	const static locale locUTF8("en_US.UTF-8");
	const static locale locUTF16LE(
		locale("en_US.UTF-8"), 
		new std::codecvt_utf8_utf16<wchar_t, 0x10ffffUL, std::little_endian>()
	); // I can call "new" because the locale's destructors deletes the facet.

	static std::map<const char*, ReadFileData> openedFiles;

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

	/* LOW-LEVEL FILE HANDLING */
#ifdef _WIN32 // Win32
	#define _OPEN_FILE_(filename, fd) _sopen_s(&fd, filename, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD)
#else // POSIX
	#define _OPEN_FILE_(filename, fd) ((fd = open(filename, O_RDONLY)) == -1)
	#define _read read /* POSIX form */
	#define _close close /* POSIX form */
#endif

#ifndef _WIN32
	typedef int HANDLE;
#endif

	/* FUNCTION DECLARATIONS */
	static bool ExistsFromCharArrayWindows(const char* filename);
	static filesize_t SizeFromCharArrayWindows(const char* filename);
	static bool ReadWindows(ReadFileData& rfd);
	static HANDLE OpenHandleFromCharArrayWindows(const char* filename);
	static bool ExistsFromWchar_tArrayWindows(const wchar_t* filename);
	static filesize_t SizeFromWchar_tArrayWindows(const wchar_t* filename);
	static HANDLE OpenHandleFromWchar_tArrayWindows(const wchar_t* filename);
	static bool ExistsFromCharArrayPOSIX(const char* filename);
	static filesize_t SizeFromCharArrayPOSIX(const char* filename);
	static bool ReadPOSIX(ReadFileData& rfd);

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
		HANDLE file = OpenHandleFromCharArrayWindows(filename);
		if (file == INVALID_HANDLE_VALUE) return -1;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		CloseHandle(file);
		return filesize_t(res.QuadPart);
#endif
	}

	static HANDLE OpenHandleFromCharArrayWindows(const char* filename)
	{
#ifdef UNICODE // LPCWSTR
		wchar_t* converted = Toolbox::ToWchar_t(filename);
		auto result = OpenHandleFromWchar_tArrayWindows(converted);
		delete[] converted;
		return result;
#else // LPCSTR
		return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
#endif
	}

	static bool ReadWindows(ReadFileData& rfd)
	{
		if (rfd.fileHandle == INVALID_HANDLE_VALUE)
			return nullptr;

		rfd.mappingHandle = CreateFileMapping(rfd.fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
		if (rfd.mappingHandle == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(rfd.fileHandle);
			return false;
		}

		rfd.memptr = (const char*)MapViewOfFile(rfd.mappingHandle, FILE_MAP_READ, 0, 0, 0);
		if (rfd.memptr == NULL)
		{
			CloseHandle(rfd.mappingHandle);
			CloseHandle(rfd.fileHandle);
			return false;
		}
		return true;
	}

#ifdef UNICODE
	static filesize_t SizeFromWchar_tArrayWindows(const wchar_t* filename)
	{
		HANDLE file = OpenHandleFromWchar_tArrayWindows(filename);
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

	static HANDLE OpenHandleFromWchar_tArrayWindows(const wchar_t* filename)
	{
		return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
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

	static bool ReadPOSIX(ReadFileData& rfd)
	{
		rfd.memptr = (const char*)mmap(NULL, rfd.size, PROT_READ, MAP_PRIVATE, rfd.fd, 0);
		if (rfd.memptr == (void*)-1)
		{
			_close(rfd.fd);
			return false;
		}
		return true;
	}
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

	const char* Read(const char* filename)
	{
		ReadFileData rfd;
		bool result;

#ifdef _WIN32
		rfd.fileHandle = OpenHandleFromCharArrayWindows(filename);
		result = ReadWindows(rfd);
#else
		rfd.size = Size(filename);
		if (rfd.size == -1 || rfd.size == 0)
			result = false;
		else
		{
			if (_OPEN_FILE_(filename, rfd.fd))
				result = false;
			else
				result = ReadPOSIX(rfd);
		}
#endif

		if (result)
		{
			openedFiles[rfd.memptr] = rfd;
			return rfd.memptr;
		}
		else
			return nullptr;
		
	}

#if defined _WIN32 && defined UNICODE
	const char* Read(const wchar_t* filename)
	{
		ReadFileData rfd;
		bool result;

		rfd.fileHandle = OpenHandleFromWchar_tArrayWindows(filename);
		result = ReadWindows(rfd);

		if (result)
		{
			openedFiles[rfd.memptr] = rfd;
			return rfd.memptr;
		}
		else
			return nullptr;
	}
#endif

#if defined _WIN32 && defined UNICODE
#endif
	void Read_Close(const char* content)
	{
		auto iterToContent = openedFiles.find(content);
		if (iterToContent != openedFiles.end())
		{
			// File is found, release its data.
			ReadFileData& rfd = iterToContent->second;
#ifdef _WIN32
			UnmapViewOfFile(content);
			CloseHandle(rfd.mappingHandle);
			CloseHandle(rfd.fileHandle);
#else
			munmap((void*)rfd.memptr, rfd.size);
			_close(rfd.fd);
#endif
			openedFiles.erase(iterToContent);
		}
		return;
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
