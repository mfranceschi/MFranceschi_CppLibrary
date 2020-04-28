//
// Created by mfran on 28/04/2020.
//

#if defined(_WIN32)

#include "WindowsAPIHelper.hpp"
#include "StringSafePlaceHolder.hpp"
#include "CommandHelper.hpp"
#include <Windows.h>

/// Use this handle as a sample for inheritable handles.
static SECURITY_ATTRIBUTES securityAttributesForInheritableHandles {
        sizeof(SECURITY_ATTRIBUTES),
        nullptr,
        true
};

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

HANDLE ProcessInputStream_None::getHandle() const {
    return GetStdHandle(STD_INPUT_HANDLE);
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

HANDLE ProcessOutputStream_Keep::getHandle() const {
    return GetStdHandle( STD_OUTPUT_HANDLE);
}

HANDLE ProcessErrorStream_Keep::getHandle() const {
    return GetStdHandle( STD_ERROR_HANDLE);
}

void ProcessOutputStream_Kill::beforeStart() {
    nulHandle = CreateFile(
            TEXT("NUL"),
            FILE_GENERIC_WRITE,
            FILE_SHARE_READ,
            &securityAttributesForInheritableHandles,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_DEVICE,
            nullptr);
}

void ProcessOutputStream_Kill::afterStop() {
    CloseHandle(nulHandle);
}

HANDLE ProcessOutputStream_Kill::getHandle() const {
    return nulHandle;
}

ProcessOutputStream_Export::ProcessOutputStream_Export(bool append, const File::SFilename_t& filename)  :
        APPEND(append), filename(filename) {}

void ProcessOutputStream_Export::beforeStart() {
    DWORD dwDesiredAccess = FILE_GENERIC_WRITE;
    if (APPEND) {
        dwDesiredAccess |= FILE_APPEND_DATA;
    }

    fileHandle = CreateFile(
            filename.c_str(),
            dwDesiredAccess,
            FILE_SHARE_READ,
            &securityAttributesForInheritableHandles,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

    if (APPEND) {
        SetFilePointer(fileHandle, 0, nullptr, FILE_END);
    }
}

void ProcessOutputStream_Export::afterStop() {
    CloseHandle(fileHandle);
}

HANDLE ProcessOutputStream_Export::getHandle() const {
    return fileHandle;
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

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
#endif
