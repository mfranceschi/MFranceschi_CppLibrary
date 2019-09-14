//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes

#include <algorithm>
#include <codecvt>
#include <fcntl.h>
#include <locale>
#include <map>
#include <mutex>

#include "File.hpp"

#ifdef _WIN32
#pragma warning( disable: 26444) // Warning that occurs when using imbue.
#include <io.h>
#include "Toolbox.hpp"
#include <Windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <cassert>

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
	public:
		/* Data members. */
		const char* memptr; // Holds the file data.
		filesize_t size; // Size of the file.

#ifdef _WIN32
		HANDLE fileHandle; // File HANDLE
		HANDLE mappingHandle; // File Mapping HANDLE
#else
		int fd; // File descriptor
#endif

		/* Default constructor. */
		ReadFileData() : memptr(nullptr), size(0ul)
#ifdef _WIN32
			, fileHandle(0), mappingHandle(0)
#else
			, fd(0)
#endif
		{}

	};

//-------------------------------------------------------------- Constants

	const static locale locUTF8("en_US.UTF-8");
	const static locale locUTF16LE(
		locale("en_US.UTF-8"), 
		new std::codecvt_utf8_utf16<wchar_t, 0x10ffffUL, std::little_endian>()
	); // I can call "new" because the locale's destructors deletes the facet.

	static std::map<const char*, ReadFileData>openedFiles;
	static std::mutex openedFilesMutex;

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

	/* LOW-LEVEL FILE HANDLING */
#ifdef _WIN32 // Win32
	#ifdef UNICODE
		#define WIN_OPEN_FCT _wsopen_s
	#else
		#define WIN_OPEN_FCT _sopen_s
	#endif

	#define _OPEN_FILE_(filename, fd) WIN_OPEN_FCT(&fd, filename, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD)
#else // POSIX
	#define _OPEN_FILE_(filename, fd) ((fd = open(filename, O_RDONLY)) == -1)
	#define _read read /* POSIX form */
	#define _close close /* POSIX form */
#endif

#ifdef _WIN32
	static HANDLE OpenHandleWindows(filename_t filename)
	{
		return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
#endif

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	bool Delete(filename_t filename, bool fileonly)
	{
#ifdef _WIN32 // Win32
		if (fileonly)
			return DeleteFile(filename);
		else
			return DeleteFile(filename) ? true : RemoveDirectory(filename);
#else // POSIX
		if (fileonly)
			return !unlink(filename);
		else
			return !remove(filename);
#endif
	}

	bool Exists(filename_t filename)
	{
#ifdef _WIN32 // Win32
		DWORD attr = GetFileAttributes(filename);
		return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
#else // POSIX
		struct stat t;
		return !stat(filename, &t);
#endif
	}

	bool IsDir(filename_t filename)
	{
#ifdef _WIN32 // Win32
		DWORD attrs = GetFileAttributes(filename);
		return (attrs == INVALID_FILE_ATTRIBUTES) ? false : attrs & FILE_ATTRIBUTE_DIRECTORY;
#else // POSIX
		struct stat s;
		return !stat(filename, &s) && S_ISDIR(s.st_mode);
#endif
	}

	bool IsEmpty(filename_t filename, unsigned int charsToRead)
	{
		int file;
		bool forReturn;

		if (_OPEN_FILE_(filename, file))
			forReturn = true;
		else
		{
			char* content = new char[charsToRead];
			forReturn = static_cast<unsigned int>(_read(file, content, charsToRead)) == charsToRead;
			delete[] content;
			_close(file);
		}

		return forReturn;
	}

	void Open(ifstream& ifs, filename_t filename,
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

	filesize_t Size(filename_t filename)
	{
#ifdef _WIN32 // Win32
		HANDLE file = OpenHandleWindows(filename);
		if (file == INVALID_HANDLE_VALUE) return 0;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		CloseHandle(file);
		return filesize_t(res.QuadPart);
#else // POSIX
		struct stat t;
		if (stat(filename, &t)) return 0;
		return filesize_t(t.st_size);
#endif
	}

	encoding_t Encoding(filename_t filename)
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

			if (bits[0] == '\xff' && bits[1] == '\xfe')
				forReturn = ENC_UTF16LE;
			else if (ret_read == 3 && bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf')
				forReturn = ENC_UTF8;
			else
				forReturn = ENC_DEFAULT;

			_close(file);
		}
		return forReturn;
	}

	bool CreateFolder(filename_t filename)
	{
#ifdef _WIN32
		return CreateDirectory(filename, NULL);
#else
		return !mkdir(filename, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
	}

	const char* Read(filename_t filename)
	{
		ReadFileData rfd;
		rfd.size = Size(filename);
		if (rfd.size == 0)
			return nullptr;

		openedFilesMutex.lock();
#ifdef _WIN32
		rfd.fileHandle = OpenHandleWindows(filename);
		if (rfd.fileHandle == INVALID_HANDLE_VALUE)
		{
			openedFilesMutex.unlock();
			return nullptr;
		}

		rfd.mappingHandle = CreateFileMapping(rfd.fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
		if (rfd.mappingHandle == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(rfd.fileHandle);
			openedFilesMutex.unlock();
			return nullptr;
		}

		rfd.memptr = (const char*)MapViewOfFile(rfd.mappingHandle, FILE_MAP_READ, 0, 0, 0);
		if (rfd.memptr == NULL)
		{
			CloseHandle(rfd.mappingHandle);
			CloseHandle(rfd.fileHandle);
			openedFilesMutex.unlock();
			return nullptr;
		}
#else
		if (_OPEN_FILE_(filename, rfd.fd))
			return nullptr;
		rfd.memptr = (const char*)mmap(NULL, rfd.size, PROT_READ, MAP_PRIVATE, rfd.fd, 0);
		if (rfd.memptr == (void*)-1)
		{
			_close(rfd.fd);
			return nullptr;
		}
#endif

		openedFiles [rfd.memptr] = rfd;
		openedFilesMutex.unlock();
		return rfd.memptr;		
	}

	bool Read_Close(const char* content)
	{
		openedFilesMutex.lock();
		bool found = false;

		auto iterToContent = openedFiles.find(content);
		if (iterToContent != openedFiles.end())
		{
			found = true;
			// File is found, release its data.
			ReadFileData& rfd = iterToContent->second;
#ifdef _WIN32
			assert(UnmapViewOfFile(content));
			assert(CloseHandle(rfd.mappingHandle));
			assert(CloseHandle(rfd.fileHandle));
#else
			munmap((void*)rfd.memptr, rfd.size);
			_close(rfd.fd);
#endif
			openedFiles.erase(iterToContent);
		}

		openedFilesMutex.unlock();
		return found;
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
