
#include "MF/Filesystem.hpp"

#include <algorithm>
#include <array>
#include <codecvt>
#include <cstdarg>
#include <sstream>

#include "FilesystemOSHelper.hpp"
#include "MF/Strings.hpp"

#if MF_WINDOWS
#    if defined(_MSC_VER)
#        pragma warning(disable : 26444) // Warning that occurs when using imbue.
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

        const static locale LOCALE_UTF8("");
        const static locale LOCALE_UTF16LE(
            locale(""),
            new std::codecvt_utf8_utf16<
                wchar_t,
                0x10ffffUL,
                std::little_endian>()); // I can call "new" because the locale's destructor deletes
                                        // the facet.

        template <class CharT>
        static std::unique_ptr<std::basic_ifstream<CharT>> internalOpenFile(
            const std::basic_string<CharT> &filename, FileEncoding_e encoding) {
            auto ptr = std::make_unique<std::basic_ifstream<CharT>>();

            switch (encoding) {
                case FileEncoding_e::ENC_UTF8:
                    ptr->open(filename.c_str());
                    ptr->imbue(LOCALE_UTF8);
                    ptr->seekg(3);
                    break;
                case FileEncoding_e::ENC_UTF16LE:
                    ptr->open(filename.c_str(), ios_base::binary);
                    ptr->imbue(LOCALE_UTF16LE);
                    ptr->seekg(2, ios_base::beg);
                    break;
                case FileEncoding_e::ENC_DEFAULT:
                    ptr->open(filename.c_str(), ios_base::binary);
                    break;
                default:
                    throw std::invalid_argument(
                        "Invalid encoding value: " + std::to_string(static_cast<int>(encoding)));
            }

            return ptr;
        }

        std::unique_ptr<std::ifstream> openFile(const Filename_t &filename) {
            return internalOpenFile(filename, getFileEncoding(filename));
        }

        std::unique_ptr<std::ifstream> openFile(
            const Filename_t &filename, FileEncoding_e encoding) {
            return internalOpenFile(filename, encoding);
        }

        static FileEncoding_e parseBitsAndFindEncoding(
            const std::array<char, NBR_BITS_TO_READ_ENCODING> &bits) {
            if (bits[0] == '\xff' && bits[1] == '\xfe') {
                return FileEncoding_e::ENC_UTF16LE;
            }

            if (bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf') {
                return FileEncoding_e::ENC_UTF8;
            }

            return FileEncoding_e::ENC_DEFAULT;
        }

        FileEncoding_e getFileEncoding(const Filename_t &filename) {
            std::array<char, NBR_BITS_TO_READ_ENCODING> bits{};
            osReadFileToBuffer(filename, bits.data(), NBR_BITS_TO_READ_ENCODING);
            return parseBitsAndFindEncoding(bits);
        }

        std::vector<Filename_t> listFilesInDirectory(const Filename_t &folder) {
            const bool addFileSeparatorAtTheEnd = !MF::Strings::endsWith(folder, FILE_SEPARATOR);

            std::vector<Filename_t> result;
            osGetDirectoryContents(
                addFileSeparatorAtTheEnd ? folder + FILE_SEPARATOR : folder, result);
            std::sort(result.begin(), result.end());
            return result;
        }

#if MF_WINDOWS
        std::unique_ptr<std::wifstream> openFile(const WideFilename_t &filename) {
            return internalOpenFile(filename, getFileEncoding(filename));
        }

        std::unique_ptr<std::wifstream> openFile(
            const WideFilename_t &filename, FileEncoding_e encoding) {
            return internalOpenFile(filename, encoding);
        }

        FileEncoding_e getFileEncoding(const WideFilename_t &filename) {
            std::array<char, NBR_BITS_TO_READ_ENCODING> bits{};
            osReadFileToBuffer(filename, bits.data(), NBR_BITS_TO_READ_ENCODING);
            return parseBitsAndFindEncoding(bits);
        }

        std::vector<WideFilename_t> listFilesInDirectory(const WideFilename_t &folder) {
            bool const addFileSeparatorAtTheEnd =
                !MF::Strings::endsWith(folder, FILE_SEPARATOR_WIDE);

            std::vector<WideFilename_t> result;
            osGetDirectoryContents(
                addFileSeparatorAtTheEnd ? folder + FILE_SEPARATOR_WIDE : folder, result);
            std::sort(result.begin(), result.end());
            return result;
        }
#endif
    } // namespace Filesystem
} // namespace MF
