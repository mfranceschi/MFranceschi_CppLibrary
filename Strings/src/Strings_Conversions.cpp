//
// Created by MartinF on 16/10/2022.
//

#include "MF/Strings.hpp"

#include <Windows.h>

#include <cassert>

namespace MF
{
    namespace Strings
    {
        namespace Conversions
        {
#if MF_WINDOWS
            static std::wstring multiByteToWideCharWrapper(
                const std::string& input, UINT codePage) {
                // Thanks to https://stackoverflow.com/a/6693107 !
                const int wchars_num = MultiByteToWideChar(
                    codePage, // Source encoding basically.
                    MB_ERR_INVALID_CHARS, // Flag recommended for UTF-8.
                    input.data(), // C-string source
                    -1, // Indicates that the source C-string is null-terminated.
                    nullptr, // No use for now.
                    0); // We ask this function to return the number of bytes to allocate.
                assert(wchars_num != 0); // TODO error case.

                std::wstring wstr(wchars_num, 0);
                const int returnValue =
                    MultiByteToWideChar(codePage, 0, input.c_str(), -1, &wstr[0], wchars_num);
                assert(returnValue != 0); // TODO error case.
                return wstr;
            }

            static std::string wideCharToMultiByteWrapper(
                const std::wstring& input, UINT codePage) {
                const int nbChars = WideCharToMultiByte(
                    codePage, // Target encoding basically.
                    WC_ERR_INVALID_CHARS, // Flag recommended for UTF-8.
                    input.data(), // C-string source
                    -1, // Indicates that the source C-string is null-terminated.
                    nullptr, // No use for now.
                    0, // We ask this function to return the number of bytes to allocate.
                    nullptr, // Default char, NULL since UTF-8.
                    nullptr); // Not used.
                assert(nbChars != 0); // TODO error case.

                std::string str(nbChars, 0);
                const int returnValue = WideCharToMultiByte(
                    codePage, 0, input.c_str(), -1, &str[0], nbChars, nullptr, nullptr);
                assert(returnValue != 0); // TODO error case.
                return str;
            }
#endif

            std::wstring ansiToWideChar(const std::string& input) {
                if (input.empty()) {
                    return std::wstring();
                }
#if MF_WINDOWS
                return multiByteToWideCharWrapper(input, CP_ACP);
#else
                return L"";
#endif
            }

            std::wstring utf8ToWideChar(const std::string& input) {
                if (input.empty()) {
                    return std::wstring();
                }
#if MF_WINDOWS
                return multiByteToWideCharWrapper(input, CP_UTF8);
#else
                return L"";
#endif
            }

            std::string wideCharToAnsi(const std::wstring& input) {
                if (input.empty()) {
                    return std::string();
                }
#if MF_WINDOWS
                return wideCharToMultiByteWrapper(input, CP_ACP);
#else
                return "";
#endif
            }

            std::string wideCharToUtf8(const std::wstring& input) {
                if (input.empty()) {
                    return std::string();
                }
#if MF_WINDOWS
                return wideCharToMultiByteWrapper(input, CP_UTF8);
#else
                return "";
#endif
            }
        } // namespace Conversions
    } // namespace Strings
} // namespace MF
