//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes

#include <codecvt>
#include <fcntl.h>
#include "File.hpp"
#include <locale>
#include <map>
#include <mutex>
#include <sys/stat.h>

#ifdef _WIN32
    #pragma warning( disable: 26444) // Warning that occurs when using imbue.
    #include <io.h>
    #include "Toolbox.hpp"
    #include <Windows.h>
#else
    #include <dirent.h>
    #include <sys/mman.h>
    #include <unistd.h>
#endif

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

constexpr static size_t NBR_BITS_TO_READ_ENCODING = 3;

namespace File
{
/////////////////////////////////////////////////////////////////  PRIVATE

//------------------------------------------------------------------ Types

	// Data structure used to store information about files opened with Open.
	struct ReadFileData {
		/* Data members. */
		const char* memoryPointer = nullptr; // Holds the file data.
		filesize_t size = 0ul; // Size of the file.

#ifdef _WIN32
		HANDLE fileHandle = nullptr; // File HANDLE
		HANDLE mappingHandle = nullptr; // File Mapping HANDLE
#else
		int fd = -1; // File descriptor
#endif
	};

//-------------------------------------------------------------- Constants

	const static locale LOCALE_UTF8("");
	const static locale LOCALE_UTF16LE(
		locale(""),
		new std::codecvt_utf8_utf16<wchar_t, 0x10ffffUL, std::little_endian>()
	); // I can call "new" because the locale's destructors deletes the facet.

	static std::map<const char*, ReadFileData>openedFiles;
	static std::mutex openedFilesMutex;

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

    // Returns true on success.
	static inline bool open_file(File::filename_t filename, int& fd) { 
	#ifdef _WIN32
			#ifdef UNICODE
				#define WIN_OPEN_FCT _wsopen_s
			#else
				#define WIN_OPEN_FCT _sopen_s
			#endif
			return !WIN_OPEN_FCT(&fd, filename, _O_RDONLY | O_BINARY, _SH_DENYWR, _S_IREAD);
			#undef WIN_OPEN_FCT
		#else
			fd = open(filename, O_RDONLY);
			return fd != -1;
		#endif
	}

	/* LOW-LEVEL FILE HANDLING */
#ifdef _WIN32 // Windows
	static inline HANDLE OpenHandleWindows(filename_t filename) {
		return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	}
	static inline bool IsADirFromAttributesWindows(DWORD dword) {
	    return dword & FILE_ATTRIBUTE_DIRECTORY;
	}
#else
	#define _read read /* POSIX form */
	#define _close close /* POSIX form */
#endif

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	bool Delete(filename_t filename, bool fileOnly)
	{
#ifdef _WIN32 // Win32
		if (fileOnly)
			return DeleteFile(filename);
		else
			return DeleteFile(filename) ? true : RemoveDirectory(filename);
#else // POSIX
		if (fileOnly)
			return !unlink(filename);
		else
			return !remove(filename);
#endif
	}

	bool Exists(filename_t filename)
	{
#ifdef _WIN32 // Win32
		DWORD attr = GetFileAttributes(filename);
		return !(attr == INVALID_FILE_ATTRIBUTES || IsADirFromAttributesWindows(attr));
#else // POSIX
		struct stat t{};
		return !stat(filename, &t);
#endif
	}

	bool IsDir(filename_t filename)
	{
#ifdef _WIN32 // Win32
		DWORD attrs = GetFileAttributes(filename);
		return (attrs == INVALID_FILE_ATTRIBUTES) ? false : IsADirFromAttributesWindows(attrs);
#else // POSIX
		struct stat s{};
		return !stat(filename, &s) & S_ISDIR(s.st_mode);
#endif
	}

	bool IsEmpty(filename_t filename, int charsToRead)
	{
		int file;
		bool forReturn = false;

		if (!open_file(filename, file))
			forReturn = true;
		else
		{
		    if (charsToRead >= 0) {
                char* content = new char[charsToRead];
                int chars_just_read = _read(file, content, charsToRead);
                forReturn = chars_just_read != charsToRead;
                delete[] content;
		    }
			_close(file);
		}

		return forReturn;
	}

	bool Open(ifstream& ifs, filename_t filename,
		encoding_t encoding)
	{
		ifs.close();
		if (encoding == encoding_t::ENC_ERROR)
			encoding = File::Encoding(filename);

		if (encoding == encoding_t::ENC_UTF8)
		{
			ifs.open(filename);
			ifs.imbue(LOCALE_UTF8);
			ifs.seekg(3);
			return true;
		}
		else if (encoding == encoding_t::ENC_UTF16LE)
		{
			ifs.open(filename, ios_base::binary);
			ifs.imbue(LOCALE_UTF16LE);
			ifs.seekg(2, ios_base::beg);
			return true;
		}
		else if (encoding == encoding_t::ENC_DEFAULT)
		{
			ifs.open(filename, ios_base::binary);
			return true;
		}
		else // Encoding is unknown
			return false;
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
		struct stat t{};
		if (stat(filename, &t)) return 0;
		return filesize_t(t.st_size);
#endif
	}

	encoding_t Encoding(filename_t filename)
	{
		int file;
		encoding_t forReturn;

		if (!open_file(filename, file))
			forReturn = encoding_t::ENC_ERROR;
		else
		{
			char bits[NBR_BITS_TO_READ_ENCODING];
			int ret_read = _read(file, bits, NBR_BITS_TO_READ_ENCODING);
			
			if (ret_read != NBR_BITS_TO_READ_ENCODING)
				forReturn = encoding_t::ENC_ERROR;
			else if (bits[0] == '\xff' && bits[1] == '\xfe')
				forReturn = encoding_t::ENC_UTF16LE;
			else if (bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf')
				forReturn = encoding_t::ENC_UTF8;
			else
				forReturn = encoding_t::ENC_DEFAULT;

			_close(file);
		}
		return forReturn;
	}

	bool CreateFolder(filename_t filename)
	{
#ifdef _WIN32
		return CreateDirectory(filename, nullptr);
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

		rfd.mappingHandle = CreateFileMapping(rfd.fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (rfd.mappingHandle == nullptr || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(rfd.fileHandle);
			openedFilesMutex.unlock();
			return nullptr;
		}

		rfd.memoryPointer = (const char*)MapViewOfFile(rfd.mappingHandle, FILE_MAP_READ, 0, 0, 0);
		if (rfd.memoryPointer == nullptr)
		{
			CloseHandle(rfd.mappingHandle);
			CloseHandle(rfd.fileHandle);
			openedFilesMutex.unlock();
			return nullptr;
		}
#else
		if (!open_file(filename, rfd.fd))
			return nullptr;
		rfd.memoryPointer = (const char*)mmap(nullptr, rfd.size, PROT_READ, MAP_PRIVATE, rfd.fd, 0);
		if (rfd.memoryPointer == (void*)-1)
		{
			_close(rfd.fd);
			return nullptr;
		}
#endif

		openedFiles [rfd.memoryPointer] = rfd;
		openedFilesMutex.unlock();
		return rfd.memoryPointer;
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
			UnmapViewOfFile(content);
			CloseHandle(rfd.mappingHandle);
			CloseHandle(rfd.fileHandle);
#else
			munmap((void*)rfd.memoryPointer, rfd.size);
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
		case encoding_t::ENC_UTF16LE:
			os << "UTF-16LE";
			break;
		case encoding_t::ENC_UTF8:
			os << "UTF-8";
			break;
		case encoding_t::ENC_ERROR:
			os << "<encoding-error>";
			break;
		case encoding_t::ENC_DEFAULT:
			os << "<encoding-unknown>";
			break;
		}
		return os;
	}

	str_filename_t GetCWD()
	{
	    File::filename_t result_of_syscall;
#ifdef _WIN32
#ifdef UNICODE
		result_of_syscall = _wgetcwd(nullptr, 0);
#else
		result_of_syscall = _getcwd(nullptr, 0);
#endif
#else
        result_of_syscall = getcwd(nullptr, 0);
#endif
		str_filename_t to_return = result_of_syscall;
		free((void *) result_of_syscall);
		return to_return;
	}


    std::vector<File::str_filename_t> FilesInDirectory(filename_t folder)
    {
        // Declarations
        std::vector<File::str_filename_t> result;
        File::str_filename_t tempFilename;
        static File::filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
        static File::filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
#ifdef _WIN32
        WIN32_FIND_DATA wfd;
		HANDLE hFind;
        File::str_filename_t tempFolderName = folder;
        tempFolderName += MAKE_FILE_NAME "*";
#else
        DIR * d;
        dirent * dir_entry;
#endif

#ifdef _WIN32
        hFind = FindFirstFile(tempFolderName.c_str(), &wfd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                // If it is a directory, then remove "." and ".." or append an ending backslash.
                if (IsADirFromAttributesWindows(wfd.dwFileAttributes)) {
                    tempFilename = wfd.cFileName;
                    if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                        continue;
                    }
                    else {
                        tempFilename.append(FILE_SEPARATOR);
                    }
                    result.push_back(tempFilename);
                }

                else {
                    result.emplace_back(wfd.cFileName);
                }
            } while (FindNextFile(hFind, &wfd));
            FindClose(hFind);
        }
#else

        d = opendir(folder);
        if (d) {
            while ((dir_entry = readdir(d)) != nullptr) {
                tempFilename = dir_entry->d_name;
                if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                    continue;
                } else if (File::IsDir((std::string(folder) + tempFilename).c_str())) {
                    tempFilename.append(FILE_SEPARATOR);
                }
                result.emplace_back(tempFilename);
            }
            closedir(d);
        }
#endif

        return result;
    }




} 
