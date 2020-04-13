//
// Created by mfran on 13/04/2020.
//

#if defined(_WIN32)

#include <Windows.h>
#include "WindowsAPIHelper.hpp"
#include <Strsafe.h>

void _WindowsShowErrorMessage(const char* functionName) {
    // Source: https://docs.microsoft.com/fr-fr/windows/win32/debug/retrieving-the-last-error-code

    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, nullptr );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)functionName) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    functionName, dw, lpMsgBuf);
    MessageBox(nullptr, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

int _WindowsGetExitCodeCommand(ProcessHandle& processHandle) {
    DWORD exitCode;
    if (GetExitCodeProcess(processHandle, &exitCode)) {
        return static_cast<int>(exitCode);
    } else {
        _WindowsShowErrorMessage("GetExitCodeProcess");
        return 3; // Will never be called because of "ExitProcess".
    }
}

bool _WindowsCreateCommand(const std::string& commandString, ProcessHandle& processHandle) {
    const char* commandStringPointer = commandString.c_str();
    char* newCommandString = new char[commandString.size() + 1];
    auto copyResult = StringCchCopy(newCommandString, commandString.size() + 1, commandStringPointer);
    if ( FAILED(copyResult) ) {
        if (copyResult == STRSAFE_E_INVALID_PARAMETER) {
            _WindowsShowErrorMessage("StringCchCopy <invalid param>");
        } else {
            _WindowsShowErrorMessage("StringCchCopy <insufficient buffer>");
        }
    }

    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processInformation;
    ZeroMemory(&startupinfo, sizeof(startupinfo));
    startupinfo.cb = sizeof(startupinfo);
    ZeroMemory(&processInformation, sizeof(processInformation));
    bool createProcessResult = CreateProcess(
            nullptr,    // No module name (use command line)
            newCommandString,
            nullptr,// Process handle not inheritable
            nullptr,// Thread handle not inheritable
            FALSE,// Set handle inheritance to FALSE
            0,// No creation flags
            nullptr,// Use parent's environment block
            nullptr,// Use parent's starting directory
            &startupinfo,
            &processInformation
    );
    if (!createProcessResult) {
        _WindowsShowErrorMessage("CreateProcess");
    }
    processHandle = processInformation.hProcess;
    CloseHandle(processInformation.hThread);
    _WindowsGetExitCodeCommand(processHandle);
    return createProcessResult;
}

void _WindowsWaitForProcess(ProcessHandle& processHandle) {
    WaitForSingleObject(processHandle, INFINITE);
}

void _WindowsReturnLaterCommand(HANDLE& processHandle, unsigned int duration) {
    WaitForSingleObject(processHandle, duration);
    _WindowsReturnNowProcess(processHandle);
}

void _WindowsReturnNowProcess(ProcessHandle& processHandle) {
    TerminateProcess(processHandle, -1);
    _WindowsWaitForProcess(processHandle);
}

// ///////////////////////////////////////////////////////////////
// /////////////////// FILE HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

bool _WindowsDeleteFile(File::filename_t filename) {
    return DeleteFile(filename);
}

bool _WindowsDeleteDirectory(File::filename_t directoryName) {
    return RemoveDirectory(directoryName);
}

bool _WindowsFileExists(File::filename_t filename) {
    DWORD attr = GetFileAttributes(filename);
    return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool _WindowsDirectoryExists(File::filename_t directoryName) {
    DWORD attr = GetFileAttributes(directoryName);
    return (attr == INVALID_FILE_ATTRIBUTES) ? false : (attr & FILE_ATTRIBUTE_DIRECTORY);
}

File::file_size_t _WindowsGetFileSize(File::filename_t filename) {
    HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return 0;
    LARGE_INTEGER res;
    GetFileSizeEx(file, &res);
    CloseHandle(file);
    return static_cast<File::file_size_t>(res.QuadPart);
}

bool _WindowsCreateDirectory(File::filename_t directoryName) {
    return CreateDirectory(directoryName, nullptr);
}

File::str_filename_t _WindowsGetCurrentWorkingDirectory() {
    DWORD nBufferLength = GetCurrentDirectory(0, nullptr);
    auto lpBuffer = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nBufferLength));
    GetCurrentDirectory(nBufferLength, lpBuffer);
    File::str_filename_t returnValue(lpBuffer);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    return returnValue;
}

void _WindowsGetDirectoryContents(File::filename_t directoryName, std::vector<File::str_filename_t>& result) {
    // todo
}
#endif