//
// Created by MartinF on 10/04/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_WINDOWS_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_WINDOWS_COMMONS_HPP

#if MF_WINDOWS
#    include "MF/Command.hpp"
#    include "MF/LightWindows.hpp"

namespace MF
{
    namespace Command
    {
        using StreamItem = HANDLE;
        using ProcessItem = HANDLE;

        enum class OutputStream_e { StdOut, StdErr };

        struct CommandComponent {
            /// This will be called right before the command is started.
            virtual void beforeStart() {
            }

            /// This will be called right after the command is started.
            virtual void afterStart() {
            }

            /// This will be called right before we initiate the process stopping procedure.
            virtual void beforeStop() {
            }

            /// This will be called right after the process is stopped.
            virtual void afterStop() {
            }

            virtual ~CommandComponent() = default;
        };

        struct ConsoleOutputChoice {
            virtual ~ConsoleOutputChoice() = default;
        };

        struct ConsoleInputChoice {
            virtual ~ConsoleInputChoice() = default;
        };

        struct ConsoleOutputChoice_Windows : ConsoleOutputChoice, CommandComponent {
            virtual StreamItem getStreamItem(OutputStream_e) const = 0;
        };

        struct ConsoleInputChoice_Windows : ConsoleInputChoice, CommandComponent {
            virtual StreamItem getStreamItem() const = 0;
        };

        std::vector<TCHAR> makeCommandLine(
            const Filename_t *executable, const std::vector<Filename_t> *arguments);

        void populateStartupInfo(const CommandCall &commandCall, STARTUPINFO &startupinfo);

        SECURITY_ATTRIBUTES &getInheritableSecAttr();

        int getExitCode(ProcessItem processItem);

        void makeHandleInheritable(HANDLE handle, bool yesOrNo);
    } // namespace Command
} // namespace MF

#endif

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_WINDOWS_COMMONS_HPP
