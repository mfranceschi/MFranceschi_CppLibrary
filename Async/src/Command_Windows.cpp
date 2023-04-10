//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS

#    include <cassert>
#    include <iostream>

#    include "Command_Windows_commons.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
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
            LPTSTR lpCurrentDirectory =
                nullptr; // We use the parent's current working directory - TODO customize?
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

        CommandOver runCommandAndWait(const CommandCall &commandCall) {
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).beforeStart();
            ProcessItem processItem = doCreateProcess(commandCall);
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).afterStart();

            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).beforeStop();
            assert(WaitForSingleObject(processItem, 1000) == WAIT_OBJECT_0);
            static_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice)).afterStop();

            DWORD exitCode;
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                GetExitCodeProcess(processItem, &exitCode) == FALSE);
            return {static_cast<int>(exitCode)};
        }
    } // namespace Command
} // namespace MF

#endif
