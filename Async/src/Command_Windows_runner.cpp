//
// Created by MartinF on 15/04/2023.
//

#include "Command_Windows_commons.hpp"
#include "MF/Command.hpp"
#include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        struct StatefulCommandBase {
            StatefulCommandBase(
                ConsoleInputChoice_Windows &stdInChoice,
                ConsoleOutputChoice_Windows &stdOutChoice,
                ConsoleOutputChoice_Windows &stdErrChoice)
                : stdInChoice(stdInChoice), stdOutChoice(stdOutChoice), stdErrChoice(stdErrChoice) {
            }

            virtual ProcessItem start() {
                throw std::runtime_error("Invalid call to " + std::string(__func__));
            }

            virtual void kill(int exitCode) {
                throw std::runtime_error("Invalid call to " + std::string(__func__));
            }

            virtual bool isRunning() {
                return false;
            }

            virtual bool isDone() {
                return false;
            }

            virtual const CommandOver &getCommandOver() {
                throw std::runtime_error("Invalid call to " + std::string(__func__));
            }

            virtual bool waitFor(std::chrono::milliseconds duration) {
                throw std::runtime_error("Invalid call to " + std::string(__func__));
            }

            virtual ~StatefulCommandBase() = default;

            ConsoleInputChoice_Windows &stdInChoice;
            ConsoleOutputChoice_Windows &stdOutChoice;
            ConsoleOutputChoice_Windows &stdErrChoice;
        };

        struct StatefulCommandBase_NotStartedYet_Base : StatefulCommandBase {
            StatefulCommandBase_NotStartedYet_Base(
                const std::shared_ptr<ConsoleInputChoice_Windows> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice_Windows> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice_Windows> &stdErrChoice)
                : StatefulCommandBase(*stdInChoice, *stdOutChoice, *stdErrChoice) {
                ZeroMemory(&processInformation, sizeof(processInformation));
                processInformation.hProcess = INVALID_HANDLE_VALUE;
            }

            virtual ProcessItem start() override = 0;

            ~StatefulCommandBase_NotStartedYet_Base() override = default;

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

        struct StatefulCommandBase_NotStartedYet_Char : StatefulCommandBase_NotStartedYet_Base {
            StatefulCommandBase_NotStartedYet_Char(const CommandCall &commandCall)
                : StatefulCommandBase_NotStartedYet_Base(
                      std::static_pointer_cast<ConsoleInputChoice_Windows>(commandCall.stdInChoice),
                      std::static_pointer_cast<ConsoleOutputChoice_Windows>(
                          commandCall.stdOutChoice),
                      std::static_pointer_cast<ConsoleOutputChoice_Windows>(
                          commandCall.stdErrChoice)),
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

            ~StatefulCommandBase_NotStartedYet_Char() override = default;

           private:
            std::vector<char> commandLine;
            const Filename_t currentDirectory;
            STARTUPINFOA startupInfo{0};
        };

        struct StatefulCommandBase_NotStartedYet_WideChar : StatefulCommandBase_NotStartedYet_Base {
            StatefulCommandBase_NotStartedYet_WideChar(const WideCommandCall &commandCall)
                : StatefulCommandBase_NotStartedYet_Base(
                      std::static_pointer_cast<ConsoleInputChoice_Windows>(commandCall.stdInChoice),
                      std::static_pointer_cast<ConsoleOutputChoice_Windows>(
                          commandCall.stdOutChoice),
                      std::static_pointer_cast<ConsoleOutputChoice_Windows>(
                          commandCall.stdErrChoice)),
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

            ~StatefulCommandBase_NotStartedYet_WideChar() override = default;

           private:
            std::vector<wchar_t> commandLine;
            const WideFilename_t currentDirectory;
            STARTUPINFOW startupInfo{0};
        };

        struct StatefulCommandBase_Running : StatefulCommandBase {
            StatefulCommandBase_Running(
                ProcessItem processItem,
                const std::shared_ptr<ConsoleInputChoice_Windows> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice_Windows> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice_Windows> &stdErrChoice)
                : StatefulCommandBase(*stdInChoice, *stdOutChoice, *stdErrChoice),
                  processHandle(processItem) {
            }

            void kill(int exitCode) override {
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    TerminateProcess(processHandle, exitCode) == FALSE);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    WaitForSingleObject(processHandle, INFINITE) != WAIT_OBJECT_0);
            }

            bool isRunning() override {
                DWORD exitCode;
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    !GetExitCodeProcess(processHandle, &exitCode));
                return exitCode == STILL_ACTIVE;
            }

            bool isDone() override {
                return !isRunning();
            }

            bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) override {
                DWORD result = WaitForSingleObject(
                    processHandle,
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

            ~StatefulCommandBase_Running() override {
                afterStop();
            }

           private:
            void afterStop() {
                stdInChoice.afterStop();
                stdOutChoice.afterStop();
                stdErrChoice.afterStop();
            }

            // Data of the created process
            const HANDLE processHandle = INVALID_HANDLE_VALUE;
        };

        struct StatefulCommandBase_Over : StatefulCommandBase {
            StatefulCommandBase_Over(
                ProcessItem processItem,
                const std::shared_ptr<ConsoleInputChoice_Windows> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice_Windows> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice_Windows> &stdErrChoice)
                : StatefulCommandBase(*stdInChoice, *stdOutChoice, *stdErrChoice),
                  processHandle(processItem) {
                DWORD exitCode;
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    !GetExitCodeProcess(processHandle, &exitCode));
                commandOver.exitCode = exitCode;
            }

            bool isDone() override {
                return true;
            }

            const CommandOver &getCommandOver() override {
                return commandOver;
            }

            ~StatefulCommandBase_Over() = default;

           private:
            // Data of the created process
            const HANDLE processHandle;

            // Data of the terminated process
            CommandOver commandOver{-1};
        };

        struct CommandRunner_Windows : CommandRunner {
            CommandRunner_Windows(const CommandCall &commandCall) {
                stdInChoice =
                    std::static_pointer_cast<ConsoleInputChoice_Windows>(commandCall.stdInChoice);
                stdOutChoice =
                    std::static_pointer_cast<ConsoleOutputChoice_Windows>(commandCall.stdOutChoice);
                stdErrChoice =
                    std::static_pointer_cast<ConsoleOutputChoice_Windows>(commandCall.stdErrChoice);

                statefulCommandBase =
                    std::make_unique<StatefulCommandBase_NotStartedYet_Char>(commandCall);
            }

            CommandRunner_Windows(const WideCommandCall &commandCall) {
                stdInChoice =
                    std::static_pointer_cast<ConsoleInputChoice_Windows>(commandCall.stdInChoice);
                stdOutChoice =
                    std::static_pointer_cast<ConsoleOutputChoice_Windows>(commandCall.stdOutChoice);
                stdErrChoice =
                    std::static_pointer_cast<ConsoleOutputChoice_Windows>(commandCall.stdErrChoice);

                statefulCommandBase =
                    std::make_unique<StatefulCommandBase_NotStartedYet_WideChar>(commandCall);
            }

            CommandRunner &start() override {
                processHandle = statefulCommandBase->start();
                statefulCommandBase = std::make_unique<StatefulCommandBase_Running>(
                    processHandle, stdInChoice, stdOutChoice, stdErrChoice);
                return *this;
            }

            CommandRunner &kill(int exitCode) override {
                statefulCommandBase->kill(exitCode);
                statefulCommandBase = std::make_unique<StatefulCommandBase_Over>(
                    processHandle, stdInChoice, stdOutChoice, stdErrChoice);
                return *this;
            }

            bool isRunning() override {
                return statefulCommandBase->isRunning();
            }

            bool isDone() override {
                return statefulCommandBase->isDone();
            }

            const CommandOver &getCommandOver() override {
                return statefulCommandBase->getCommandOver();
            }

            bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) override {
                bool result = statefulCommandBase->waitFor(duration);
                if (result) {
                    statefulCommandBase = std::make_unique<StatefulCommandBase_Over>(
                        processHandle, stdInChoice, stdOutChoice, stdErrChoice);
                }
                return result;
            }

            std::uintmax_t getHandle() override {
                return reinterpret_cast<uintmax_t>(processHandle);
            }

            ~CommandRunner_Windows() {
                closeH(processHandle);
            }

           private:
            std::unique_ptr<StatefulCommandBase> statefulCommandBase;

            // Console choices
            std::shared_ptr<ConsoleInputChoice_Windows> stdInChoice;
            std::shared_ptr<ConsoleOutputChoice_Windows> stdOutChoice;
            std::shared_ptr<ConsoleOutputChoice_Windows> stdErrChoice;

            // Data of the created process
            ProcessItem processHandle = INVALID_HANDLE_VALUE;
        };

        std::shared_ptr<CommandRunner> runCommandAsync(const CommandCall &commandCall) {
            return std::make_shared<CommandRunner_Windows>(commandCall);
        }

        CommandOver runCommandAndWait(const CommandCall &commandCall) {
            auto cmd = runCommandAsync(commandCall);
            cmd->start().waitFor();
            return cmd->getCommandOver();
        }
    } // namespace Command
} // namespace MF
