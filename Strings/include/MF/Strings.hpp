//
// Created by MartinF on 16/10/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP
#define MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP

#include <string>
#include <vector>

namespace MF
{
    namespace Strings
    {
        namespace Conversions
        {
            std::wstring ansiToWideChar(const std::string& input);
            std::wstring utf8ToWideChar(const std::string& input);

            std::string wideCharToAnsi(const std::wstring& input);
            std::string wideCharToUtf8(const std::wstring& input);
        } // namespace Conversions

        namespace Chars
        {
            bool isBlank(char value);
            bool isBlank(wchar_t value);
        } // namespace Chars

        template <class CharT>
        bool contains(
            const std::basic_string<CharT>& input, const std::basic_string<CharT>& substring) {
            return input.find(substring) != std::string ::npos;
        }

        template <typename CharT>
        bool isBlank(const std::basic_string<CharT>& input) {
            for (const CharT& charFromInput : input) {
                if (!Chars::isBlank(charFromInput)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * Same as java.lang.String.split(String regex),
         * except that it's not a regex but a substring matching.
         */
        template <typename CharT>
        std::vector<std::basic_string<CharT>> split(
            const std::basic_string<CharT>& input,
            const std::basic_string<CharT>& splitBySubstring = "\n") {
            std::vector<std::basic_string<CharT>> result;
            size_t position = 0;
            while (position != std::string::npos) {
                auto position2 = input.find(splitBySubstring, position);
                result.push_back(input.substr(position, position2 - position));
                position = (position2 == std::string::npos) ? position2
                                                            : position2 + splitBySubstring.length();
            }

            while (result[result.size() - 1] == std::basic_string<CharT>()) {
                result.pop_back();
            }
            return result;
        }

        template <class CharT>
        bool startsWith(
            const std::basic_string<CharT>& input, const std::basic_string<CharT>& substring) {
            for (std::size_t i = 0; i < substring.length(); i++) {
                if (input[i] != substring[i]) {
                    return false;
                }
            }
            return true;
        }

        template <class CharT>
        bool endsWith(
            const std::basic_string<CharT>& input, const std::basic_string<CharT>& substring) {
            for (std::size_t i = 0; i < substring.length(); i++) {
                if (input[input.length() - substring.length() + i] != substring[i]) {
                    return false;
                }
            }
            return true;
        }
    } // namespace Strings
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP
