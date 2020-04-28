//
// Created by mfran on 13/04/2020.
//

#if defined(_WIN32)

#include <Windows.h>
#include "WindowsAPIHelper.hpp"
#include "StringSafePlaceHolder.hpp"

void Windows_ShowErrorMessage(const char* functionName) {
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

const wchar_t* Windows_ConvertString(const char* utf8String) {
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

void Windows_MakeHandleInheritable(void* handle, bool inherit) {
    SetHandleInformation(handle, HANDLE_FLAG_INHERIT, inherit ? HANDLE_FLAG_INHERIT : 0);
}

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

int Windows_GetExitCodeCommand(Windows_ProcessHandle& processHandle) {
    DWORD exitCode;
    if (GetExitCodeProcess(processHandle, &exitCode)) {
        return static_cast<int>(exitCode);
    } else {
        Windows_ShowErrorMessage("GetExitCodeProcess");
        return 3; // Will never be called because of "ExitProcess".
    }
}

void Windows_WaitForProcess(Windows_ProcessHandle& processHandle) {
    WaitForSingleObject(processHandle, INFINITE);
}

void Windows_ReturnLaterCommand(HANDLE& processHandle, unsigned int duration) {
    WaitForSingleObject(processHandle, duration);
    Windows_ReturnNowProcess(processHandle);
}

void Windows_ReturnNowProcess(Windows_ProcessHandle& processHandle) {
    TerminateProcess(processHandle, -1);
    Windows_WaitForProcess(processHandle);
}

// ///////////////////////////////////////////////////////////////
// /////////////////// FILE HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

bool Windows_DeleteFile(File::Filename_t filename) {
    return DeleteFile(filename);
}

bool Windows_DeleteDirectory(File::Filename_t directoryName) {
    return RemoveDirectory(directoryName);
}

bool Windows_FileExists(File::Filename_t filename) {
    DWORD attr = GetFileAttributes(filename);
    return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool Windows_DirectoryExists(File::Filename_t directoryName) {
    DWORD attr = GetFileAttributes(directoryName);
    return (attr == INVALID_FILE_ATTRIBUTES) ? false : (attr & FILE_ATTRIBUTE_DIRECTORY);
}

File::Filesize_t Windows_GetFileSize(File::Filename_t filename) {
    HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return 0;
    LARGE_INTEGER res;
    GetFileSizeEx(file, &res);
    CloseHandle(file);
    return static_cast<File::Filesize_t>(res.QuadPart);
}

bool Windows_CreateDirectory(File::Filename_t directoryName) {
    return CreateDirectory(directoryName, nullptr);
}

File::SFilename_t Windows_GetCurrentWorkingDirectory() {
    DWORD nBufferLength = GetCurrentDirectory(0, nullptr);
    auto lpBuffer = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nBufferLength * sizeof(TCHAR)));
    GetCurrentDirectory(nBufferLength, lpBuffer);
    File::SFilename_t returnValue(lpBuffer);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    return returnValue;
}

void Windows_GetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t>& result) {
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

const Windows_ReadFileData* Windows_OpenFile(File::Filename_t filename) {
    auto rfd = static_cast<Windows_ReadFileData*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Windows_ReadFileData)));
    rfd->size = Windows_GetFileSize(filename);
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

void Windows_CloseReadFileData(const Windows_ReadFileData* readFileData) {
    UnmapViewOfFile(readFileData->contents);
    CloseHandle(readFileData->mappingHandle);
    CloseHandle(readFileData->fileHandle);
    HeapFree(GetProcessHeap(), 0, (void*) readFileData);
}

void Windows_CloseReadFileData(const File::ReadFileData* readFileData) {
    (void)(readFileData); // to avoid "unused parameter warning
}

int Windows_ReadFileToBuffer(File::Filename_t filename, char* buffer, File::Filesize_t bufferSize) {
    Windows_FileHandle fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    DWORD numberOfBytesRead;
    bool returnValue = ReadFile(fileHandle, buffer, bufferSize, &numberOfBytesRead, nullptr);
    CloseHandle(fileHandle);

    return returnValue ? static_cast<int>(numberOfBytesRead) : -1;
}
#endif
