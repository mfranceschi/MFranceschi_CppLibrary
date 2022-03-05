//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes

#include <stdarg.h>
#include <codecvt>
#include "MF/Filesystem.hpp"
#include <locale>
#include <sys/stat.h>

#if defined(_WIN32)
#   if defined(_MSC_VER)
#       pragma warning( disable: 26444) // Warning that occurs when using imbue.
#   endif
#   if defined(UNICODE)
#       include "Toolbox.hpp"
#   endif

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
using Soss_t = std::wostringstream;
#else
using Soss_t = std::ostringstream;
#endif

namespace File {
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

    SFilename_t MakeFilename(bool absolute, bool isDirectory, int number, ...) {
        (void) (absolute);
        using ArgumentsType = File::Filename_t;
        Soss_t oss;
        va_list argsList;
        va_start(argsList, number);
        ArgumentsType currentArg;

#if !defined(_WIN32)
        // On UNIX, if the given path is intended to be absolute,
        // we prepend a FILE_SEPARATOR.
        if (absolute) {
            oss << FILE_SEPARATOR;
        }
#endif

        for (int i = 0; i < number - 1; i++) {
            currentArg = va_arg(argsList, ArgumentsType);
            oss << currentArg << FILE_SEPARATOR;
        }

        currentArg = va_arg(argsList, ArgumentsType);
        oss << currentArg;

        if (isDirectory) {
            oss << FILE_SEPARATOR;
        }
        return oss.str();
    }

    bool Delete(Filename_t filename, bool fileOnly) {
#ifdef _WIN32 // Win32
        if (fileOnly)
            return Windows_DeleteFile(filename);
        else
            return Windows_DeleteFile(filename) ? true : Windows_DeleteDirectory(filename);
#else // POSIX
        if (fileOnly)
            return Unix_DeleteFile(filename);
        else
            return Unix_DeleteFileOrDirectory(filename);
#endif
    }

    bool Exists(Filename_t filename) {
#ifdef _WIN32 // Win32
        return Windows_FileExists(filename);
#else // POSIX
        return Unix_FileExists(filename);
#endif
    }

    bool IsDir(Filename_t filename) {
#ifdef _WIN32 // Win32
        return Windows_DirectoryExists(filename);
#else // POSIX
        return Unix_DirectoryExists(filename);
#endif
    }

    bool CanReadFile(Filename_t filename, int charsToRead) {
        if (charsToRead > 0) {
            char *buffer = new char[charsToRead];
            int bytesRead;

#if defined(_WIN32)
            bytesRead = Windows_ReadFileToBuffer(filename, buffer, charsToRead);
#else
            bytesRead = Unix_ReadFileToBuffer(filename, buffer, charsToRead);
#endif

            delete[] buffer;
            return bytesRead == charsToRead;
        } else {

#if defined(_WIN32)
            return Windows_FileExists(filename);
#else
            return Unix_FileExists(filename);
#endif

        }
    }

    bool Open(ifstream &ifs, Filename_t filename,
              encoding_t encoding) {
        ifs.close();
        if (encoding == encoding_t::ENC_ERROR)
            encoding = File::Encoding(filename);

        if (encoding == encoding_t::ENC_UTF8) {
            ifs.open(filename);
            ifs.imbue(LOCALE_UTF8);
            ifs.seekg(3);
            return true;
        } else if (encoding == encoding_t::ENC_UTF16LE) {
            ifs.open(filename, ios_base::binary);
            ifs.imbue(LOCALE_UTF16LE);
            ifs.seekg(2, ios_base::beg);
            return true;
        } else if (encoding == encoding_t::ENC_DEFAULT) {
            ifs.open(filename, ios_base::binary);
            return true;
        } else // Encoding is unknown
            return false;
    }

    Filesize_t Size(Filename_t filename) {
#if defined(_WIN32) // Win32
        return Windows_GetFileSize(filename);
#else // POSIX
        return Unix_GetFileSize(filename);
#endif
    }

    encoding_t Encoding(Filename_t filename) {
        char bits[NBR_BITS_TO_READ_ENCODING];
        encoding_t forReturn;
        int readResult;

#if defined(_WIN32)
        readResult = Windows_ReadFileToBuffer(filename, bits, NBR_BITS_TO_READ_ENCODING);
#else
        readResult = Unix_ReadFileToBuffer(filename, bits, NBR_BITS_TO_READ_ENCODING);
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

    bool CreateFolder(Filename_t filename) {
#ifdef _WIN32
        return Windows_CreateDirectory(filename);
#else
        return Unix_CreateDirectory(filename);
#endif
    }

    std::ostream &operator<<(std::ostream &os, const encoding_t &enc) {
        switch (enc) {
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

    SFilename_t GetCWD() {
#ifdef _WIN32
        return Windows_GetCurrentWorkingDirectory();
#else
        return Unix_GetCurrentWorkingDirectory();
#endif
    }

    std::vector<File::SFilename_t> FilesInDirectory(Filename_t folder) {
        std::vector<File::SFilename_t> result;
#if defined(_WIN32)
        Windows_GetDirectoryContents(folder, result);
#else
        Unix_GetDirectoryContents(folder, result);
#endif
        return result;
    }
}
