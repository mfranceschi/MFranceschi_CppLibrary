//
// Created by Utilisateur on 05/11/2022.
//

#include <cwctype>

#include "MF/Strings.hpp"

namespace MF
{
    namespace Strings
    {
        namespace Chars
        {
            bool isBlank(char value) {
                return std::isblank(value) != 0;
            }

            bool isBlank(wchar_t value) {
                return std::iswblank(value) != 0;
            }

            bool isSpace(char value) {
                return isspace(value) != 0;
            }

            bool isSpace(wchar_t value) {
                return iswspace(value) != 0;
            }
        } // namespace Chars
    } // namespace Strings
} // namespace MF
