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

const wchar_t* _WindowsConvert(const char* utf8String) {
    // Thanks to https://stackoverflow.com/a/6693107 !
    int wchars_num = MultiByteToWideChar(
            CP_UTF8, // Source (and so the expected processing) is UTF-8.
            0, // Because of UTF-8.
            utf8String, // C-string source
            -1, // Indicates that the source C-string is null-terminated.
            nullptr, // No use for now.
            0); // We ask this function to return the number of bytes to allocate.

    auto* wstr = new wchar_t[wchars_num];
    MultiByteToWideChar(
            CP_UTF8, 0, utf8String, -1,
            wstr, wchars_num);
    return wstr;
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

bool _WindowsDeleteFile(File::Filename_t filename) {
    return DeleteFile(filename);
}

bool _WindowsDeleteDirectory(File::Filename_t directoryName) {
    return RemoveDirectory(directoryName);
}

bool _WindowsFileExists(File::Filename_t filename) {
    DWORD attr = GetFileAttributes(filename);
    return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool _WindowsDirectoryExists(File::Filename_t directoryName) {
    DWORD attr = GetFileAttributes(directoryName);
    return (attr == INVALID_FILE_ATTRIBUTES) ? false : (attr & FILE_ATTRIBUTE_DIRECTORY);
}

File::Filesize_t _WindowsGetFileSize(File::Filename_t filename) {
    HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return 0;
    LARGE_INTEGER res;
    GetFileSizeEx(file, &res);
    CloseHandle(file);
    return static_cast<File::Filesize_t>(res.QuadPart);
}

bool _WindowsCreateDirectory(File::Filename_t directoryName) {
    return CreateDirectory(directoryName, nullptr);
}

File::SFilename_t _WindowsGetCurrentWorkingDirectory() {
    DWORD nBufferLength = GetCurrentDirectory(0, nullptr);
    auto lpBuffer = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nBufferLength));
    GetCurrentDirectory(nBufferLength, lpBuffer);
    File::SFilename_t returnValue(lpBuffer);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    return returnValue;
}

void _WindowsGetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t>& result) {
    File::SFilename_t tempFilename;
    static File::Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
    static File::Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    File::SFilename_t tempFolderName = directoryName;
    tempFolderName += MAKE_FILE_NAME "*";
    hFind = FindFirstFile(tempFolderName.c_str(), &wfd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // If it is a directory, then remove "." and ".." or append an ending backslash.
            if ((wfd.dwFileAttributes == INVALID_FILE_ATTRIBUTES) ? false : (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                tempFilename = wfd.cFileName;
                if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                    continue;
                }
                else {
                    tempFilename += MAKE_FILE_NAME FILE_SEPARATOR;
                }
                result.push_back(tempFilename);
            }

            else {
                result.emplace_back(wfd.cFileName);
            }
        } while (FindNextFile(hFind, &wfd));
        FindClose(hFind);
    }
}

const _WindowsReadFileData* _WindowsOpenFile(File::Filename_t filename) {
    auto rfd = static_cast<_WindowsReadFileData*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(_WindowsReadFileData)));
    rfd->size = _WindowsGetFileSize(filename);
    if (rfd->size == 0) {
        HeapFree(GetProcessHeap(), 0, rfd);
        return nullptr;
    }

    rfd->fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (rfd->fileHandle == INVALID_HANDLE_VALUE)
    {
        HeapFree(GetProcessHeap(), 0, rfd);
        return nullptr;
    }

    rfd->mappingHandle = CreateFileMapping(rfd->fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (rfd->mappingHandle == nullptr || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(rfd->fileHandle);
        HeapFree(GetProcessHeap(), 0, rfd);
        return nullptr;
    }

    rfd->contents = (const char*)MapViewOfFile(rfd->mappingHandle, FILE_MAP_READ, 0, 0, 0);
    if (rfd->contents == nullptr)
    {
        CloseHandle(rfd->mappingHandle);
        CloseHandle(rfd->fileHandle);
        HeapFree(GetProcessHeap(), 0, rfd);
        return nullptr;
    }

    return rfd;
}

void _WindowsCloseReadFileData(const _WindowsReadFileData* rfd) {
    UnmapViewOfFile(rfd->contents);
    CloseHandle(rfd->mappingHandle);
    CloseHandle(rfd->fileHandle);
    HeapFree(GetProcessHeap(), 0, (void*) rfd);
}

void _WindowsCloseReadFileData(const File::ReadFileData* readFileData) {
    (void)(readFileData); // to avoid "unused parameter warning
}

int _WindowsReadFileToBuffer(File::Filename_t filename, char* buffer, File::Filesize_t bufferSize) {
    FileHandle fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    DWORD numberOfBytesRead;
    bool returnValue = ReadFile(fileHandle, buffer, bufferSize, &numberOfBytesRead, nullptr);
    CloseHandle(fileHandle);

    return returnValue ? static_cast<int>(numberOfBytesRead) : -1;
}
#endif
