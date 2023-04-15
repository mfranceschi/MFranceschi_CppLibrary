//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS

#    include "Command_Windows_commons.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        static void beforeStart(const CommandCall &commandCall) {
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).beforeStart();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice)).beforeStart();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdErrChoice)).beforeStart();
        }

        static void afterStart(const CommandCall &commandCall) {
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).afterStart();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice)).afterStart();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdErrChoice)).afterStart();
        }

        static void beforeStop(const CommandCall &commandCall) {
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).beforeStop();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice)).beforeStop();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdErrChoice)).beforeStop();
        }

        static void afterStop(const CommandCall &commandCall) {
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).afterStop();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice)).afterStop();
            static_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdErrChoice)).afterStop();
        }

        static ProcessItem doCreateProcess(const CommandCall &commandCall) {
            LPCTCH lpApplicationName = nullptr;
            std::vector<TCHAR> commandLine =
                makeCommandLine(&commandCall.executable, &commandCall.arguments);
            LPTSTR lpCommandLine = commandLine.data();

            LPSECURITY_ATTRIBUTES lpProcessAttributes = nullptr; // No specific security attribute
            LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr; // No specific security attribute
            bool bInheritHandles = true; // Handles are inherited
            DWORD dwCreationFlags = 0; // Creation flags
            LPVOID lpEnvironment = nullptr; // We use the parent's environment
            LPCTSTR lpCurrentDirectory = commandCall.workingDirectory.empty()
                                             ? nullptr
                                             : commandCall.workingDirectory.c_str();
            STARTUPINFO startupinfo;
            populateStartupInfo(commandCall, startupinfo);

            PROCESS_INFORMATION processInformation;
            ZeroMemory(&processInformation, sizeof(processInformation));
            // TODO Use the right handles (?)

            bool createProcessResult = CreateProcess(
                lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, &startupinfo,
                &processInformation);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!createProcessResult);

            CloseHandle(processInformation.hThread);
            return processInformation.hProcess;
        }

        void doStopProcess(ProcessItem processItem, std::chrono::milliseconds waitFor) {
            DWORD result = WaitForSingleObject(
                processItem,
                waitFor == std::chrono::milliseconds::zero() ? INFINITE : waitFor.count());
            if (result == WAIT_OBJECT_0) {
                // It means that the process terminated.
                return;
            }

            if (result == WAIT_TIMEOUT) {
                throw std::runtime_error("Timeout reached!");
            }

            // Some other error happened. Throwing.
            throw MF::SystemErrors::Win32::getCurrentSystemError();
        }

        CommandOver runCommandAndWait(
            const CommandCall &commandCall, std::chrono::milliseconds waitFor) {
            /*
            beforeStart(commandCall);
            ProcessItem processItem = doCreateProcess(commandCall);
            afterStart(commandCall);

            beforeStop(commandCall);
            doStopProcess(processItem, waitFor);
            afterStop(commandCall);

            return {getExitCode(processItem)};
             */
            auto cmd = runCommandAsync(commandCall);
            cmd->start().waitFor(waitFor);
            return cmd->getCommandOverOrThrow();
        }
    } // namespace Command
} // namespace MF

#endif
