//
// Created by MartinF on 29/05/2022.
//

//
// Created by Utilisateur on 10/04/2022.
//

#include "MF/SharedLibs.hpp"

#if MF_UNIX
#    include <sys/wait.h>
#    include <unistd.h>
#endif

namespace MF
{
    namespace SharedLibs
    {
        bool CanOpen(const std::string &libName) noexcept {
#if MF_WINDOWS
            try {
                OpenExplicitly(libName);
                return true;
            } catch (const std::exception &) {
                return false;
            }
#else
            pid_t c_pid = fork();

            if (c_pid == -1) {
                // An error occurred when forking.
                // Ignoring but returning false.
                return false;
            }

            if (c_pid > 0) {
                // Parent process.
                int status = 0;
                if (waitpid(c_pid, &status, 0) != -1) {
                    if (WIFEXITED(status)) {
                        int exitStatus = WEXITSTATUS(status);
                        // I could slightly optimize and put both booleans in the "if" condition,
                        // but it would make it harder to understand what is wrong.
                        return exitStatus == EXIT_SUCCESS;
                    }
                }
                return false;
            } else {
                // Child process.
                OpenExplicitly(libName);
                exit(EXIT_SUCCESS);
            }
            return true;
#endif
        }
    } // namespace SharedLibs
} // namespace MF
