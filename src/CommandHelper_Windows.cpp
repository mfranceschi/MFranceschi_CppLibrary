//
// Created by mfran on 28/04/2020.
//

#include "WindowsAPIHelper.hpp"
#include "StringSafePlaceHolder.hpp"
#include "CommandHelper.hpp"
#include <Windows.h>

HANDLE ProcessInputStream_None::getHandle() const {
    return GetStdHandle(STD_INPUT_HANDLE);
}

HANDLE ProcessOutputStream_Keep::getHandle() const {
    return GetStdHandle( OUTPUT_AND_NOT_ERROR ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
}

void CommandRunner::internalStart() {
// Define all parameters required by the CreateProcess function.
    auto lpApplicationName = executable->c_str();
    TCHAR* lpCommandLine;
    {
        // Fill the "Command line" string
        File::OSStream_t osStream;
        for (const auto& arg : *arguments) {
            osStream << arg << " ";
        }
        auto streamOutput = osStream.str();

        const TCHAR* commandStringPointer = streamOutput.c_str();
        lpCommandLine = new TCHAR[streamOutput.size() + 1];
        auto copyResult = StringCchCopy(lpCommandLine, streamOutput.size() + 1, commandStringPointer);

        if ( FAILED(copyResult) ) {
            if (copyResult == STRSAFE_E_INVALID_PARAMETER) {
                Windows_ShowErrorMessage("StringCchCopy <invalid param>");
            } else {
                Windows_ShowErrorMessage("StringCchCopy <insufficient buffer>");
            }
        }
    }
    LPSECURITY_ATTRIBUTES lpProcessAttributes = nullptr; // No specific security attribute
    LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr; // No specific security attribute
    bool bInheritHandles = true; // Handles are inherited
    DWORD dwCreationFlags = 0; // Creation flags
    LPVOID lpEnvironment = nullptr; // We use the parent's environment
    const TCHAR* lpCurrentDirectory = nullptr; // We use the parent's current working directory
    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processInformation;
    LPSTARTUPINFO lpStartupInfo = &startupinfo;
    LPPROCESS_INFORMATION lpProcessInformation = &processInformation;
    {
        ZeroMemory(&startupinfo, sizeof(startupinfo));
        startupinfo.cb = sizeof(startupinfo);
        startupinfo.dwFlags |= STARTF_USESTDHANDLES;
        startupinfo.hStdOutput = processOutputStream->getHandle();
        startupinfo.hStdError = processErrorStream->getHandle();
        startupinfo.hStdInput = processInputStream->getHandle();
        ZeroMemory(&processInformation, sizeof(processInformation));
        // By default, we have nothing in these structs
    }

    // TODO Use the right handles

    bool createProcessResult = CreateProcess(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation
    );
    if (!createProcessResult) {
        Windows_ShowErrorMessage("CreateProcess");
        // TODO remove later
    }

    processHandle = processInformation.hProcess;
    CloseHandle(processInformation.hThread);
}

void CommandRunner::internalStop() {

}
