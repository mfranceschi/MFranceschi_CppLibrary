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
#   include "UnixAPIHelper.hpp"
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
        (void)(absolute);
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
			return _UnixDeleteFile(filename);
		else
			return _UnixDeleteFileOrDirectory(filename);
#endif
	}

	bool Exists(Filename_t filename)
	{
#ifdef _WIN32 // Win32
	    return _WindowsFileExists(filename);
#else // POSIX
		return _UnixFileExists(filename);
#endif
	}

	bool IsDir(Filename_t filename)
	{
#ifdef _WIN32 // Win32
		return _WindowsDirectoryExists(filename);
#else // POSIX
		return _UnixDirectoryExists(filename);
#endif
	}

	bool CanReadFile(Filename_t filename, int charsToRead)
	{
        if (charsToRead > 0) {
            char *buffer = new char[charsToRead];
            int bytesRead;

#if defined(_WIN32)
            bytesRead = _WindowsReadFileToBuffer(filename, buffer, charsToRead);
#else
            bytesRead = _UnixReadFileToBuffer(filename, buffer, charsToRead);
#endif

            delete[] buffer;
            return bytesRead == charsToRead;
        } else {

#if defined(_WIN32)
            return _WindowsFileExists(filename);
#else
            return _UnixFileExists(filename);
#endif

        }
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
#if defined(_WIN32) // Win32
		return _WindowsGetFileSize(filename);
#else // POSIX
		return _UnixGetFileSize(filename);
#endif
	}

	encoding_t Encoding(Filename_t filename)
    {
	    // TODO fix
        char bits[NBR_BITS_TO_READ_ENCODING];
        encoding_t forReturn;
        int readResult;

#if defined(_WIN32)
        readResult = _WindowsReadFileToBuffer(filename, bits, NBR_BITS_TO_READ_ENCODING);
#else
        readResult = _UnixReadFileToBuffer(filename, bits, NBR_BITS_TO_READ_ENCODING);
#endif
        if (readResult != NBR_BITS_TO_READ_ENCODING) {
            forReturn = File::encoding_t::ENC_ERROR;
        } else if (bits[0] == '\xff' && bits[1] == '\xfe') {
            forReturn = encoding_t::ENC_UTF16LE;
        } else if (bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf') {
            forReturn = encoding_t::ENC_UTF8;
        } else {
            forReturn = encoding_t::ENC_DEFAULT;
        }
		return forReturn;
	}

	bool CreateFolder(Filename_t filename)
	{
#ifdef _WIN32
		return _WindowsCreateDirectory(filename);
#else
		return _UnixCreateDirectory(filename);
#endif
	}

	const ReadFileData* Read(Filename_t filename)
	{
#ifdef _WIN32
		return _WindowsOpenFile(filename);
#else
		return _UnixOpenFile(filename);
#endif
	}

	void Read_Close(const ReadFileData* content)
	{
#if defined(_WIN32)
	    return _WindowsCloseReadFileData(content);
#else
		return _UnixCloseReadFileData(content);
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
        return _UnixGetCurrentWorkingDirectory();
#endif
	}

    std::vector<File::SFilename_t> FilesInDirectory(Filename_t folder) {
        std::vector<File::SFilename_t> result;
#if defined(_WIN32)
        _WindowsGetDirectoryContents(folder, result);
#else
        _UnixGetDirectoryContents(folder, result);
#endif
        return result;
    }
}
