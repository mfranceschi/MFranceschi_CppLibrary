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
                return std::isblank(value);
            }
            bool isBlank(wchar_t value) {
                return std::iswblank(value) != 0;
            }
        } // namespace Chars
    } // namespace Strings
} // namespace MF
