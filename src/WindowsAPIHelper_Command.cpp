//
// Created by mfran on 24/04/2020.
//

#include "WindowsAPIHelper.hpp"
#include <Windows.h>
#include "StringSafePlaceHolder.hpp"

bool _WindowsCreateCommand(const CommandCall& commandCall, ProcessHandle& processHandle) {
    /*
     * Sources:
     * https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output
     * https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes
     */

    // Define all parameters required by the CreateProcess function.
    auto lpApplicationName = commandCall.executable.c_str();
    TCHAR* lpCommandLine;
    {
        File::OSStream_t osStream;
        for (const auto& arg : commandCall.arguments) {
            osStream << arg << " ";
        }
        auto streamOutput = osStream.str();

        const TCHAR* commandStringPointer = streamOutput.c_str();
        lpCommandLine = new TCHAR[streamOutput.size() + 1];
        auto copyResult = StringCchCopy(lpCommandLine, streamOutput.size() + 1, commandStringPointer);

        if ( FAILED(copyResult) ) {
            if (copyResult == STRSAFE_E_INVALID_PARAMETER) {
                _WindowsShowErrorMessage("StringCchCopy <invalid param>");
            } else {
                _WindowsShowErrorMessage("StringCchCopy <insufficient buffer>");
            }
        }
    }
    LPSECURITY_ATTRIBUTES lpProcessAttributes = nullptr; // No specific security attribute
    LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr; // No specific security attribute
    bool bInheritHandles = false; // Handles are inherited TODO WHAT
    DWORD dwCreationFlags = 0; // Creation flags TODO WHAT
    LPVOID lpEnvironment = nullptr; // We use the parent's environment
    const TCHAR* lpCurrentDirectory = nullptr; // We use the parent's current working directory
    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processInformation;
    LPSTARTUPINFO lpStartupInfo = &startupinfo;
    LPPROCESS_INFORMATION lpProcessInformation = &processInformation;
    {
        ZeroMemory(&startupinfo, sizeof(startupinfo));
        startupinfo.cb = sizeof(startupinfo);
        ZeroMemory(&processInformation, sizeof(processInformation));
        // By default, we have nothing there
    }

    // Do some specific stuff.
    if (
            (commandCall.outputChoice != OutputChoice::KEEP) ||
            (commandCall.errorsChoice != ErrorChoice::KEEP) ||
            (commandCall.inputChoice != InputChoice::NONE)) {
        bInheritHandles = true;
    }

    bool createProcessResult = CreateProcess(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,// Process handle not inheritable
            lpThreadAttributes,// Thread handle not inheritable
            bInheritHandles,// Set handle inheritance to FALSE
            dwCreationFlags,// No creation flags
            lpEnvironment,// Use parent's environment block
            lpCurrentDirectory,// Use parent's starting directory
            lpStartupInfo,
            lpProcessInformation
    );
    if (!createProcessResult) {
        _WindowsShowErrorMessage("CreateProcess");
    }
    processHandle = processInformation.hProcess;
    CloseHandle(processInformation.hThread);
    _WindowsGetExitCodeCommand(processHandle);
    return createProcessResult;
}

