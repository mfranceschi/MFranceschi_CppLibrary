//
// Created by MartinF on 16/10/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP
#define MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP

#include <string>

namespace MF
{
    namespace Strings
    {
        template <class CharT>
        bool stringEndsWith(
            const std::basic_string<CharT>& input, const std::basic_string<CharT>& endToLookFor) {
            for (std::size_t i = 0; i < endToLookFor.length(); i++) {
                if (input[input.length() - endToLookFor.length() + i] != endToLookFor[i]) {
                    return false;
                }
            }
            return true;
        }
    } // namespace Strings
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_STRINGS_HPP
