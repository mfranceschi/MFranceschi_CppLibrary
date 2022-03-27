//---------- Implementation of module <File> (file File.cpp)

//--------------------------------------------------------------- Includes

#include "MF/Filesystem.hpp"

#include <codecvt>
#include <cstdarg>
#include <sstream>

#include "FilesystemOSHelper.hpp"

#if MF_WINDOWS
#    if defined(_MSC_VER)
#        pragma warning(disable : 26444) // Warning that occurs when using imbue.
#    endif
#    if defined(UNICODE)
#        include "Toolbox.hpp"
#    endif
#else
#    include <sys/mman.h>
#endif

using std::ifstream;
using std::ios_base;
using std::locale;
using std::string;

constexpr static size_t NBR_BITS_TO_READ_ENCODING = 3;

#if MF_WINDOWS && defined(UNICODE)
using Soss_t = std::wostringstream;
#else
using Soss_t = std::ostringstream;
#endif

namespace MF
{
    namespace Filesystem
    {
        /////////////////////////////////////////////////////////////////  PRIVATE

        //------------------------------------------------------------------ Types

        //-------------------------------------------------------------- Constants

        const static locale LOCALE_UTF8("");
        const static locale LOCALE_UTF16LE(
            locale(""),
            new std::codecvt_utf8_utf16<
                wchar_t,
                0x10ffffUL,
                std::little_endian>()); // I can call "new" because the locale's destructors deletes
                                        // the facet.

        //------------------------------------------------------- Static variables

        //------------------------------------------------------ Private functions

        //////////////////////////////////////////////////////////////////  PUBLIC
        //------------------------------------------------------- Public functions

        SFilename_t MakeFilename(bool absolute, bool isDirectory, int number, ...) {
            (void)(absolute);
            using ArgumentsType = Filename_t;
            Soss_t oss;
            va_list argsList;
            va_start(argsList, number);
            ArgumentsType currentArg;

#if MF_UNIX
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
            return fileOnly ? osDeleteFile(filename) : osDeleteFileOrDirectory(filename);
        }

        bool DeleteFile(Filename_t filename) {
            return osDeleteFile(filename);
        }

        bool DeleteDirectory(Filename_t filename) {
            return osDeleteDirectory(filename);
        }

        bool IsFile(Filename_t filename) {
            return osFileExists(filename);
        }

        bool IsDir(Filename_t filename) {
            return osDirectoryExists(filename);
        }

        bool IsFileReadable(Filename_t filename, Filesize_t charsToRead) {
            if (charsToRead <= 0) {
                return osFileExists(filename);
            }

            std::vector<char> buffer(charsToRead);
            int bytesRead = osReadFileToBuffer(filename, buffer.data(), charsToRead);
            return bytesRead == charsToRead;
        }

        bool OpenFile(ifstream &ifs, Filename_t filename, Encoding_t encoding) {
            ifs.close();
            if (encoding == Encoding_e::ENC_ERROR) {
                encoding = GetFileEncoding(filename);
            }

            if (encoding == Encoding_e::ENC_UTF8) {
                ifs.open(filename);
                ifs.imbue(LOCALE_UTF8);
                ifs.seekg(3);
                return true;
            } else if (encoding == Encoding_e::ENC_UTF16LE) {
                ifs.open(filename, ios_base::binary);
                ifs.imbue(LOCALE_UTF16LE);
                ifs.seekg(2, ios_base::beg);
                return true;
            } else if (encoding == Encoding_e::ENC_DEFAULT) {
                ifs.open(filename, ios_base::binary);
                return true;
            } else {
                // GetFileEncoding is unknown
                return false;
            }
        }

        Filesize_t GetFileSize(Filename_t filename) {
            return osGetFileSize(filename);
        }

        Encoding_t GetFileEncoding(Filename_t filename) {
            char bits[NBR_BITS_TO_READ_ENCODING];
            Encoding_t forReturn;
            int readResult =
                osReadFileToBuffer(filename, (char *)(&bits), NBR_BITS_TO_READ_ENCODING);

            if (readResult != NBR_BITS_TO_READ_ENCODING) {
                forReturn = Encoding_e::ENC_ERROR;
            } else if (bits[0] == '\xff' && bits[1] == '\xfe') {
                forReturn = Encoding_e::ENC_UTF16LE;
            } else if (bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf') {
                forReturn = Encoding_e::ENC_UTF8;
            } else {
                forReturn = Encoding_e::ENC_DEFAULT;
            }
            return forReturn;
        }

        bool CreateDirectory(Filename_t filename) {
            return osCreateDirectory(filename);
        }

        std::ostream &operator<<(std::ostream &theOstream, const Encoding_t &enc) {
            switch (enc) {
                case Encoding_e::ENC_UTF16LE:
                    theOstream << "UTF-16LE";
                    break;
                case Encoding_e::ENC_UTF8:
                    theOstream << "UTF-8";
                    break;
                case Encoding_e::ENC_ERROR:
                    theOstream << "<encoding-error>";
                    break;
                case Encoding_e::ENC_DEFAULT:
                    theOstream << "<encoding-unknown>";
                    break;
            }
            return theOstream;
        }

        SFilename_t GetCWD() {
            return osGetCWD();
        }

        std::vector<SFilename_t> ListFilesInDirectory(Filename_t folder) {
            std::vector<SFilename_t> result;
            osGetDirectoryContents(folder, result);
            return result;
        }

        std::unique_ptr<const WholeFileData> ReadWholeFile(Filename_t filename) {
            return osReadWholeFile(filename);
        }

        bool ReadWholeFileToString(Filename_t filename, std::string &string) {
            auto fileContents = ReadWholeFile(filename);
            if (fileContents) {
                string.assign(fileContents->contents, fileContents->size);
                return true;
            }

            return false;
        }
    } // namespace Filesystem
} // namespace MF
