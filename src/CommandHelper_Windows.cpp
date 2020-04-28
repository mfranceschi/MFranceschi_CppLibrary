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

static constexpr unsigned int BUFFER_LENGTH = 4096;

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

HANDLE ProcessInputStream_None::getHandle() const {
    return GetStdHandle(STD_INPUT_HANDLE);
}

void ProcessInputStream_String::beforeStart() {
    SECURITY_ATTRIBUTES securityAttributes {
            sizeof(SECURITY_ATTRIBUTES),
            nullptr,
            true
    };
    CreatePipe(&readPipeHandle, &writeToPipeHandle, &securityAttributes, BUFFER_LENGTH);
    Windows_MakeHandleInheritable(readPipeHandle);
}

void ProcessInputStream_String::afterStart() {
    DWORD lpWritten;
    std::string temp;
    temp.reserve(999);
    for (std::size_t i = 0; i < inputString.length(); i += 999) {
        temp = inputString.substr(i, 999);
        WriteFile(writeToPipeHandle, temp.c_str(), 999, &lpWritten, nullptr);
    }
}

void ProcessInputStream_String::afterStop() {
    CloseHandle(writeToPipeHandle);
    CloseHandle(readPipeHandle);
}

HANDLE ProcessInputStream_String::getHandle() const {
    return readPipeHandle;
}

void ProcessInputStream_FromFile::beforeStart() {
    fileHandle = CreateFile(
            filename.c_str(),
            FILE_GENERIC_READ,
            FILE_SHARE_READ,
            &securityAttributesForInheritableHandles,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
}

void ProcessInputStream_FromFile::afterStop() {
    CloseHandle(fileHandle);
}

HANDLE ProcessInputStream_FromFile::getHandle() const {
    return fileHandle;
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

void ProcessOutputStream_Export::beforeStart() {
    fileHandle = CreateFile(
            filename.c_str(),
            FILE_GENERIC_WRITE,
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

void ProcessOutputStream_Retrieve::beforeStart() {
    SECURITY_ATTRIBUTES securityAttributes {
        sizeof(SECURITY_ATTRIBUTES),
        nullptr,
        true
    };
    CreatePipe(&readPipeHandle, &writeToPipeHandle, &securityAttributes, BUFFER_LENGTH);
    Windows_MakeHandleInheritable(writeToPipeHandle);
}

void ProcessOutputStream_Retrieve::beforeStop() {
    DWORD dwRead;
    CHAR chBuf[BUFFER_LENGTH];

    if (ReadFile(readPipeHandle, chBuf, BUFFER_LENGTH - 1, &dwRead, nullptr) || (dwRead != 0)) {
        chBuf[BUFFER_LENGTH - 1] = '\0';
        oss << chBuf;
    }
}

void ProcessOutputStream_Retrieve::afterStop() {
    DWORD dwRead;
    CHAR chBuf[BUFFER_LENGTH];

    while (ReadFile(readPipeHandle, chBuf, BUFFER_LENGTH - 1, &dwRead, nullptr) || (dwRead != 0)) {
        chBuf[BUFFER_LENGTH - 1] = '\0';
        oss << chBuf;
    }
    CloseHandle(writeToPipeHandle);
    CloseHandle(readPipeHandle);
}

std::string ProcessOutputStream_Retrieve::retrieveOutput() {
    return oss.str();
}

HANDLE ProcessOutputStream_Retrieve::getHandle() const {
    return writeToPipeHandle;
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

int CommandRunner::internalGetStatusCode() {
    return Windows_GetExitCodeCommand(processHandle);
}
#endif
