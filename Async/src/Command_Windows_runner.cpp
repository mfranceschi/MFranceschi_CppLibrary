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
        struct CommandRunner_Windows : CommandRunner {
            CommandRunner_Windows(const CommandCall &commandCall) {
                commandLine = makeCommandLine(&commandCall.executable, &commandCall.arguments);
                currentDirectory = commandCall.workingDirectory;
                populateStartupInfo(commandCall, startupInfo);
                ZeroMemory(&processInformation, sizeof(processInformation));

                stdInChoice =
                    std::static_pointer_cast<ConsoleInputChoice_Windows>(commandCall.stdInChoice);
                stdOutChoice =
                    std::static_pointer_cast<ConsoleOutputChoice_Windows>(commandCall.stdOutChoice);
                stdErrChoice =
                    std::static_pointer_cast<ConsoleOutputChoice_Windows>(commandCall.stdErrChoice);
            }

            CommandRunner &start() override {
                beforeStart();

                LPCTCH lpApplicationName = nullptr;
                LPTSTR lpCommandLine = commandLine.data();

                LPSECURITY_ATTRIBUTES lpProcessAttributes =
                    nullptr; // No specific security attribute
                LPSECURITY_ATTRIBUTES lpThreadAttributes =
                    nullptr; // No specific security attribute
                bool bInheritHandles = true; // Handles are inherited
                DWORD dwCreationFlags = 0; // Creation flags
                LPVOID lpEnvironment = nullptr; // We use the parent's environment
                LPCTSTR lpCurrentDirectory =
                    currentDirectory.empty() ? nullptr : currentDirectory.c_str();

                bool createProcessResult = CreateProcess(
                    lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                    bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                    &startupInfo, &processInformation);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!createProcessResult);

                CloseHandle(processInformation.hThread);
                processHandle = processInformation.hProcess;
                started = true;

                afterStart();

                return *this;
            }

            CommandRunner &kill(int exitCode) override {
                beforeStop();
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    TerminateProcess(processHandle, exitCode) == FALSE);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    WaitForSingleObject(processHandle, INFINITE) != WAIT_OBJECT_0);
                isOver = true;
                commandOver.exitCode = exitCode;

                afterStop();

                return *this;
            }

            bool isRunning() override {
                if (!started || isOver) {
                    return false;
                }
                return storeExitCodeOrReturnFalse();
            }

            bool isDone() override {
                if (isOver) {
                    return true;
                }
                return !storeExitCodeOrReturnFalse();
            }

            const CommandOver &getCommandOverOrThrow() override {
                if (isDone()) {
                    return commandOver;
                }
                throw std::runtime_error("Process has not finished yet!");
            }

            bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) override {
                beforeStop();

                DWORD result = WaitForSingleObject(
                    processHandle,
                    duration == std::chrono::milliseconds::zero() ? INFINITE : duration.count());
                if (result == WAIT_OBJECT_0) {
                    // It means that the process terminated.
                    storeExitCodeOrReturnFalse();
                    return true;
                }

                if (result == WAIT_TIMEOUT) {
                    // It means that the process is still running when the timeout is reached.
                    return false;
                }

                // Some other error happened. Throwing.
                throw MF::SystemErrors::Win32::getCurrentSystemError();
            }

            std::uintmax_t getHandle() override {
                return reinterpret_cast<uintmax_t>(processHandle);
            }

            ~CommandRunner_Windows() {
                closeH(processHandle);
            }

           private:
            bool storeExitCodeOrReturnFalse() {
                beforeStop();

                DWORD exitCode;
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    !GetExitCodeProcess(processHandle, &exitCode));
                if (exitCode == STILL_ACTIVE) {
                    return false;
                }

                markAsOver(exitCode);
                return true;
            }

            void beforeStart() {
                stdInChoice->beforeStart();
                stdOutChoice->beforeStart();
                stdErrChoice->beforeStart();
            }

            void afterStart() {
                stdInChoice->afterStart();
                stdOutChoice->afterStart();
                stdErrChoice->afterStart();
            }

            void beforeStop() {
                stdInChoice->beforeStop();
                stdOutChoice->beforeStop();
                stdErrChoice->beforeStop();
            }

            void afterStop() {
                stdInChoice->afterStop();
                stdOutChoice->afterStop();
                stdErrChoice->afterStop();
            }

            void markAsOver(int exitCode) {
                afterStop();
                isOver = true;
                commandOver.exitCode = exitCode;
            }

            // Console choices
            std::shared_ptr<ConsoleInputChoice_Windows> stdInChoice;
            std::shared_ptr<ConsoleOutputChoice_Windows> stdOutChoice;
            std::shared_ptr<ConsoleOutputChoice_Windows> stdErrChoice;

            // Data preparing the call to CreateProcess
            STARTUPINFO startupInfo{0};
            Filename_t currentDirectory;
            PROCESS_INFORMATION processInformation{0};
            std::vector<char> commandLine;

            // Data of the created process
            HANDLE processHandle = INVALID_HANDLE_VALUE;
            bool started = false;

            // Data of the terminated process
            CommandOver commandOver{-1};
            bool isOver = false;
        };

        std::shared_ptr<CommandRunner> runCommandAsync(const CommandCall &commandCall) {
            return std::make_shared<CommandRunner_Windows>(commandCall);
        }

        CommandOver runCommandAndWait(
            const CommandCall &commandCall, std::chrono::milliseconds waitFor) {
            auto cmd = runCommandAsync(commandCall);
            if (cmd->start().waitFor(waitFor)) {
                return cmd->getCommandOverOrThrow();
            }
            throw std::runtime_error("Unfinished despite timeout!");
        }
    } // namespace Command
} // namespace MF
