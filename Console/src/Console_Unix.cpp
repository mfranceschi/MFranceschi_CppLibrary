//
// Created by MartinF on 14/01/2023.
//

#if MF_UNIX

#    include <termios.h>
#    include <unistd.h>

#    include "MF/Console.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Console
    {
        char ConsoleToolbox::getNext() {
            char buffer{0};
            struct termios infos {};

            SystemErrors::Errno::throwCurrentSystemErrorIf(tcgetattr(STDIN_FILENO, &infos) == -1);

            const auto c_lflag_copy = infos.c_lflag; // TODO

            infos.c_lflag &= ~ECHO; // Non-echo mode

            SystemErrors::Errno::throwCurrentSystemErrorIf(
                tcsetattr(STDIN_FILENO, TCSANOW, &infos) == -1);

            auto sizeRead = read(STDIN_FILENO, &buffer, sizeof(buffer));
            if (sizeRead != 1) {
                infos.c_lflag &= ECHO; // Echo mode
                tcsetattr(STDIN_FILENO, TCSANOW, &infos); // Don't check for errors.

                SystemErrors::Errno::throwCurrentSystemErrorIf(true);
            }

            return buffer;
        }
    } // namespace Console
} // namespace MF

#endif
