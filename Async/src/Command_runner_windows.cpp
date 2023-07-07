//
// Created by MartinF on 08/05/2023.
//
#if MF_WINDOWS

#    include "Command_commons.hpp"
#    include "Command_commons_windows.hpp"
#    include "MF/SystemErrors.hpp"

static constexpr UINT EXIT_CODE_FOR_KILLED = -1;

namespace MF
{
    namespace Command
    {
        struct StatefulCommand_NotStartedYet_Base : StatefulCommand_Base {
            StatefulCommand_NotStartedYet_Base(
                ConsoleInputChoice &stdInChoice,
                ConsoleOutputChoice &stdOutChoice,
                ConsoleOutputChoice &stdErrChoice)
                : StatefulCommand_Base(stdInChoice, stdOutChoice, stdErrChoice) {
                ZeroMemory(&processInformation, sizeof(processInformation));
                processInformation.hProcess = INVALID_HANDLE_VALUE;
            }

            virtual ProcessItem start() override = 0;

            ~StatefulCommand_NotStartedYet_Base() override = default;

           protected:
            void beforeStart() {
                stdInChoice.beforeStart();
                stdOutChoice.beforeStart();
                stdErrChoice.beforeStart();
            }

            void afterStart() {
                stdInChoice.afterStart();
                stdOutChoice.afterStart();
                stdErrChoice.afterStart();
            }

            PROCESS_INFORMATION processInformation{0};
        };

        struct StatefulCommand_NotStartedYet_Char : StatefulCommand_NotStartedYet_Base {
            StatefulCommand_NotStartedYet_Char(const CommandCall &commandCall)
                : StatefulCommand_NotStartedYet_Base(
                      *(commandCall.stdInChoice),
                      *(commandCall.stdOutChoice),
                      *(commandCall.stdErrChoice)),
                  commandLine(makeCommandLine(&commandCall.executable, &commandCall.arguments)),
                  currentDirectory(commandCall.workingDirectory) {
                populateStartupInfo(commandCall, startupInfo);
            }

            ProcessItem start() override {
                beforeStart();

                const char *lpApplicationName = nullptr;
                char *lpCommandLine = commandLine.data();

                LPSECURITY_ATTRIBUTES lpProcessAttributes =
                    nullptr; // No specific security attribute
                LPSECURITY_ATTRIBUTES lpThreadAttributes =
                    nullptr; // No specific security attribute
                bool bInheritHandles = true; // Handles are inherited
                DWORD dwCreationFlags = 0; // Creation flags
                LPVOID lpEnvironment = nullptr; // We use the parent's environment
                const char *lpCurrentDirectory =
                    currentDirectory.empty() ? nullptr : currentDirectory.c_str();

                bool createProcessResult = CreateProcessA(
                    lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                    bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                    &startupInfo, &processInformation);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!createProcessResult);

                CloseHandle(processInformation.hThread);

                afterStart();

                return processInformation.hProcess;
            }

            ~StatefulCommand_NotStartedYet_Char() override = default;

           private:
            std::vector<char> commandLine;
            const Filename_t currentDirectory;
            STARTUPINFOA startupInfo{0};
        };

        struct StatefulCommand_NotStartedYet_WideChar : StatefulCommand_NotStartedYet_Base {
            StatefulCommand_NotStartedYet_WideChar(const WideCommandCall &commandCall)
                : StatefulCommand_NotStartedYet_Base(
                      *(commandCall.stdInChoice),
                      *(commandCall.stdOutChoice),
                      *(commandCall.stdErrChoice)),
                  commandLine(makeCommandLine(&commandCall.executable, &commandCall.arguments)),
                  currentDirectory(commandCall.workingDirectory) {
                populateStartupInfo(commandCall, startupInfo);
            }

            ProcessItem start() override {
                beforeStart();

                const wchar_t *lpApplicationName = nullptr;
                wchar_t *lpCommandLine = commandLine.data();

                LPSECURITY_ATTRIBUTES lpProcessAttributes =
                    nullptr; // No specific security attribute
                LPSECURITY_ATTRIBUTES lpThreadAttributes =
                    nullptr; // No specific security attribute
                bool bInheritHandles = true; // Handles are inherited
                DWORD dwCreationFlags = 0; // Creation flags
                LPVOID lpEnvironment = nullptr; // We use the parent's environment
                const wchar_t *lpCurrentDirectory =
                    currentDirectory.empty() ? nullptr : currentDirectory.c_str();

                bool createProcessResult = CreateProcessW(
                    lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                    bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                    &startupInfo, &processInformation);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!createProcessResult);

                CloseHandle(processInformation.hThread);

                afterStart();

                return processInformation.hProcess;
            }

            ~StatefulCommand_NotStartedYet_WideChar() override = default;

           private:
            std::vector<wchar_t> commandLine;
            const WideFilename_t currentDirectory;
            STARTUPINFOW startupInfo{0};
        };

        struct StatefulCommand_Running : StatefulCommand_Base {
            StatefulCommand_Running(
                ProcessItem processItem,
                const std::shared_ptr<ConsoleInputChoice> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdErrChoice)
                : StatefulCommand_Base(*stdInChoice, *stdOutChoice, *stdErrChoice),
                  processItem(processItem) {
            }

            void kill() override {
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    TerminateProcess(processItem, EXIT_CODE_FOR_KILLED) == FALSE);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    WaitForSingleObject(processItem, INFINITE) != WAIT_OBJECT_0);
            }

            bool isRunning() override {
                DWORD exitCode;
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    !GetExitCodeProcess(processItem, &exitCode));
                return exitCode == STILL_ACTIVE;
            }

            bool isDone() override {
                return !isRunning();
            }

            bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) override {
                DWORD result = WaitForSingleObject(
                    processItem,
                    duration == std::chrono::milliseconds::zero() ? INFINITE : duration.count());
                if (result == WAIT_OBJECT_0) {
                    // It means that the process terminated.
                    return true;
                }

                if (result == WAIT_TIMEOUT) {
                    // It means that the process is still running when the timeout is reached.
                    return false;
                }

                // Some other error happened. Throwing.
                throw MF::SystemErrors::Win32::getCurrentSystemError();
            }

            ~StatefulCommand_Running() override {
                afterStop();
            }

           private:
            void afterStop() {
                stdInChoice.afterStop();
                stdOutChoice.afterStop();
                stdErrChoice.afterStop();
            }

            // Data of the created process
            const HANDLE processItem = INVALID_HANDLE_VALUE;
        };

        struct StatefulCommand_Over : StatefulCommand_Base {
            StatefulCommand_Over(
                ProcessItem processItem,
                const std::shared_ptr<ConsoleInputChoice> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdErrChoice)
                : StatefulCommand_Base(*stdInChoice, *stdOutChoice, *stdErrChoice),
                  processItem(processItem) {
                DWORD exitCode;
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    !GetExitCodeProcess(processItem, &exitCode));
                commandOver.exitCode = exitCode;
            }

            bool isDone() override {
                return true;
            }

            const CommandOver &getCommandOver() override {
                return commandOver;
            }

            ~StatefulCommand_Over() = default;

           private:
            // Data of the created process
            const HANDLE processItem;

            // Data of the terminated process
            CommandOver commandOver{-1};
        };

        CommandRunner_Proxy::CommandRunner_Proxy(const CommandCall &commandCall) {
            stdInChoice = commandCall.stdInChoice;
            stdOutChoice = commandCall.stdOutChoice;
            stdErrChoice = commandCall.stdErrChoice;

            statefulCommandBase = std::make_unique<StatefulCommand_NotStartedYet_Char>(commandCall);
        }

        CommandRunner_Proxy::CommandRunner_Proxy(const WideCommandCall &commandCall) {
            stdInChoice = commandCall.stdInChoice;
            stdOutChoice = commandCall.stdOutChoice;
            stdErrChoice = commandCall.stdErrChoice;

            statefulCommandBase =
                std::make_unique<StatefulCommand_NotStartedYet_WideChar>(commandCall);
        }

        CommandRunner &CommandRunner_Proxy::start() {
            processItem = statefulCommandBase->start();
            statefulCommandBase = std::make_unique<StatefulCommand_Running>(
                processItem, stdInChoice, stdOutChoice, stdErrChoice);
            return *this;
        }

        CommandRunner &CommandRunner_Proxy::kill() {
            statefulCommandBase->kill();
            statefulCommandBase = std::make_unique<StatefulCommand_Over>(
                processItem, stdInChoice, stdOutChoice, stdErrChoice);
            return *this;
        }

        bool CommandRunner_Proxy::isRunning() {
            return statefulCommandBase->isRunning();
        }

        bool CommandRunner_Proxy::isDone() {
            return statefulCommandBase->isDone();
        }

        const CommandOver &CommandRunner_Proxy::getCommandOver() {
            return statefulCommandBase->getCommandOver();
        }

        bool CommandRunner_Proxy::waitFor(std::chrono::milliseconds duration) {
            bool result = statefulCommandBase->waitFor(duration);
            if (result) {
                statefulCommandBase = std::make_unique<StatefulCommand_Over>(
                    processItem, stdInChoice, stdOutChoice, stdErrChoice);
            }
            return result;
        }

        void CommandRunner_Proxy::wait() {
            statefulCommandBase->waitFor(std::chrono::milliseconds::zero());
            statefulCommandBase = std::make_unique<StatefulCommand_Over>(
                processItem, stdInChoice, stdOutChoice, stdErrChoice);
        }

        std::uintmax_t CommandRunner_Proxy::getHandle() {
            return reinterpret_cast<uintmax_t>(processItem);
        }

        CommandRunner_Proxy::~CommandRunner_Proxy() {
            closeH(processItem);
        }
    } // namespace Command
} // namespace MF

#endif
