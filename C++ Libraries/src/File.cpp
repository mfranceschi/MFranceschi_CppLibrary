//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes

#include <stdarg.h>
#include <codecvt>
#include "File.hpp"
#include <locale>
#include <sys/stat.h>

#if defined(_WIN32)
#   pragma warning( disable: 26444) // Warning that occurs when using imbue.
#   include "Toolbox.hpp"
#   include "WindowsAPIHelper.hpp"
#else
#   include <dirent.h>
#   include <sys/mman.h>
#   include <unistd.h>
#endif

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

constexpr static size_t NBR_BITS_TO_READ_ENCODING = 3;

#if defined(_WIN32) && defined(UNICODE)
using _Soss_t = std::wostringstream;
#else
using _Soss_t = std::ostringstream;
#endif

namespace File
{
/////////////////////////////////////////////////////////////////  PRIVATE

//------------------------------------------------------------------ Types

//-------------------------------------------------------------- Constants

	const static locale LOCALE_UTF8("");
	const static locale LOCALE_UTF16LE(
		locale(""),
		new std::codecvt_utf8_utf16<wchar_t, 0x10ffffUL, std::little_endian>()
	); // I can call "new" because the locale's destructors deletes the facet.

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

    SFilename_t MakeFilename(bool absolute, int number, ...) {
        using _ArgumentsType = const char*;
        _Soss_t oss;
        va_list argsList;
        va_start(argsList, number);
        _ArgumentsType currentArg;

#if !defined(_WIN32)
        // On UNIX, if the given path is intended to be absolute,
        // we prepend a FILE_SEPARATOR.
        if (absolute) {
            oss << FILE_SEPARATOR;
        }
#endif

        for (int i = 0; i < number - 1 ; i ++) {
            currentArg = va_arg(argsList, _ArgumentsType);

#if defined(_WIN32) && defined(UNICODE)
            const wchar_t* wcharCurrentArg = _WindowsConvert(currentArg);
            oss << wcharCurrentArg << FILE_SEPARATOR;
            delete[] wcharCurrentArg;
#else
            oss << currentArg << FILE_SEPARATOR;
#endif
        }
        return oss.str();
    }

	bool Delete(Filename_t filename, bool fileOnly)
	{
#ifdef _WIN32 // Win32
		if (fileOnly)
			return _WindowsDeleteFile(filename);
		else
			return _WindowsDeleteFile(filename) ? true : _WindowsDeleteDirectory(filename);
#else // POSIX
		if (fileOnly)
			return !unlink(filename);
		else
			return !remove(filename);
#endif
	}

	bool Exists(Filename_t filename)
	{
#ifdef _WIN32 // Win32
	    return _WindowsFileExists(filename);
#else // POSIX
		struct stat t{};
		return !stat(filename, &t);
#endif
	}

	bool IsDir(Filename_t filename)
	{
#ifdef _WIN32 // Win32
		return _WindowsDirectoryExists(filename);
#else // POSIX
		struct stat s{};
		return !stat(filename, &s) & S_ISDIR(s.st_mode);
#endif
	}

	bool CanReadFile(Filename_t filename, int charsToRead)
	{
	    // TODO fix
#if defined(_WIN32)
        if (charsToRead > 0) {
            char *buffer = new char[charsToRead];
            int bytesRead = _WindowsReadFileToBuffer(filename, buffer, charsToRead);
            delete[] buffer;
            return bytesRead == charsToRead;
        } else {
            return _WindowsFileExists(filename);
        }
#else
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
#endif
	}

	bool Open(ifstream& ifs, Filename_t filename,
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

	Filesize_t Size(Filename_t filename)
	{
#ifdef _WIN32 // Win32
		return _WindowsGetFileSize(filename);
#else // POSIX
		struct stat t{};
		if (stat(filename, &t)) return 0;
		return Filesize_t(t.st_size);
#endif
	}

	encoding_t Encoding(Filename_t filename)
    {
	    // TODO fix
        char bits[NBR_BITS_TO_READ_ENCODING];
        encoding_t forReturn;

#if defined(_WIN32)
        int readResult = _WindowsReadFileToBuffer(filename, bits, NBR_BITS_TO_READ_ENCODING);
        if (readResult != NBR_BITS_TO_READ_ENCODING) {
            forReturn = File::encoding_t::ENC_ERROR;
        } else if (bits[0] == '\xff' && bits[1] == '\xfe') {
            forReturn = encoding_t::ENC_UTF16LE;
        } else if (bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf') {
            forReturn = encoding_t::ENC_UTF8;
        } else {
            forReturn = encoding_t::ENC_DEFAULT;
        }
#else

		int file;

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
#endif
		return forReturn;
	}

	bool CreateFolder(Filename_t filename)
	{
#ifdef _WIN32
		return _WindowsCreateDirectory(filename);
#else
		return !mkdir(filename, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
	}

	const ReadFileData* Read(Filename_t filename)
	{
#ifdef _WIN32
		return _WindowsOpenFile(filename);
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
	}

	void Read_Close(const ReadFileData* content)
	{
#if defined(_WIN32)
	    return _WindowsCloseReadFileData(content);
#else
		openedFilesMutex.lock();
		bool found = false;

		auto iterToContent = openedFiles.find(content);
		if (iterToContent != openedFiles.end())
		{
			found = true;
			// File is found, release its data.
			ReadFileData& rfd = iterToContent->second;
			munmap((void*)rfd.memoryPointer, rfd.size);
			_close(rfd.fd);
			openedFiles.erase(iterToContent);
		}

		openedFilesMutex.unlock();
		return found;
#endif
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

	SFilename_t GetCWD()
	{
#ifdef _WIN32
        return _WindowsGetCurrentWorkingDirectory();
#else
        File::Filename_t result_of_syscall;
        result_of_syscall = getcwd(nullptr, 0);
        SFilename_t to_return = result_of_syscall;
		free((void *) result_of_syscall);
		return to_return;
#endif
	}


    std::vector<File::SFilename_t> FilesInDirectory(Filename_t folder) {
        std::vector<File::SFilename_t> result;
#if defined(_WIN32)
        _WindowsGetDirectoryContents(folder, result);
#else
        File::SFilename_t tempFilename;
        static File::Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
        static File::Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
        DIR * d;
        dirent * dir_entry;
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
