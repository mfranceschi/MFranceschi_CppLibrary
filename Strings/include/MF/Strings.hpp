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
            // TODO use locale? such as
            // 'std::use_facet<std::ctype<CharT>>(std::locale()).tolower(ch)'
            bool isBlank(char value);
            bool isBlank(wchar_t value);

            bool isSpace(char value);
            bool isSpace(wchar_t value);

            char toUpper(char value);
            wchar_t toUpper(wchar_t value);

            char toLower(char value);
            wchar_t toLower(wchar_t value);
        } // namespace Chars

        template <class CharT>
        bool contains(
            const std::basic_string<CharT>& input, const std::basic_string<CharT>& substring) {
            return input.find(substring) != std::basic_string<CharT>::npos;
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

        template <typename CharT>
        std::vector<std::basic_string<CharT>> split(
            const std::basic_string<CharT>& input,
            const std::basic_string<CharT>& splitBySubstring = "\n") {
            std::vector<std::basic_string<CharT>> result;
            size_t position = 0;
            while (position != std::basic_string<CharT>::npos) {
                auto position2 = input.find(splitBySubstring, position);
                result.push_back(input.substr(position, position2 - position));
                position = (position2 == std::basic_string<CharT>::npos)
                               ? position2
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
            if (input.length() < substring.length()) {
                return false;
            }

            for (std::size_t i = 0; i < substring.length(); i++) {
                if (input[i] != substring[i]) {
                    return false;
                }
            }
            return true;
        }

        template <class CharT>
        bool startsWith(const std::basic_string<CharT>& input, CharT firstChar) {
            return input.empty() ? false : input[0] == firstChar;
        }

        template <class CharT>
        bool endsWith(
            const std::basic_string<CharT>& input, const std::basic_string<CharT>& substring) {
            if (input.length() < substring.length()) {
                return false;
            }

            for (std::size_t i = 0; i < substring.length(); i++) {
                if (input[input.length() - substring.length() + i] != substring[i]) {
                    return false;
                }
            }
            return true;
        }

        template <class CharT>
        bool endsWith(const std::basic_string<CharT>& input, CharT lastChar) {
            return input.empty() ? false : input[input.size() - 1] == lastChar;
        }

        template <class CharT>
        std::basic_string<CharT> strip(const CharT* input) {
            return strip(std::basic_string<CharT>(input));
        }

        template <class CharT>
        std::basic_string<CharT> strip(const std::basic_string<CharT>& input) {
            if (isBlank(input)) {
                return std::basic_string<CharT>();
            }

            std::size_t firstNonBlank = 0;
            while (Chars::isSpace(input[firstNonBlank])) {
                firstNonBlank++;
            }

            std::size_t lastBlankBlock = input.length();
            while (Chars::isSpace(input[lastBlankBlock - 1])) {
                lastBlankBlock--;
            }

            return input.substr(firstNonBlank, lastBlankBlock - firstNonBlank);
        }

        template <class CharT>
        std::basic_string<CharT> toLowerCase(const std::basic_string<CharT>& input) {
            std::basic_string<CharT> result;
            result.reserve(input.length());

            for (const CharT& charT : input) {
                result += Chars::toLower(charT);
            }
            return result;
        }

        template <class CharT>
        std::basic_string<CharT> toUpperCase(const std::basic_string<CharT>& input) {
            std::basic_string<CharT> result;
            result.reserve(input.length());

            for (const CharT& charT : input) {
                result += Chars::toUpper(charT);
            }
            return result;
        }

        template <class CharT, class Iterator_t>
        std::basic_string<CharT> join(
            const std::basic_string<CharT>& separator, Iterator_t beginIt, Iterator_t endIt) {
            // TODO test
            std::basic_string<CharT> result;
            while (beginIt != endIt) {
                result += separator;
                beginIt++;
            }
            return result.substr(0, result.length() - separator.length());
        }
    } // namespace Strings
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP
