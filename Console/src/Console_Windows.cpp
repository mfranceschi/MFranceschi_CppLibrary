//
// Created by MartinF on 14/01/2023.
//

#if MF_WINDOWS
#    include <conio.h>

#    include "MF/Console.hpp"

namespace MF
{
    namespace Console
    {
        char ConsoleToolbox::getNext() {
            return _getch();
        }
    } // namespace Console
} // namespace MF
#endif
