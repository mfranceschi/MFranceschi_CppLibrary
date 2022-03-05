//
// Created by MartinF on 05/03/2022.
//

#include "FilesystemOSHelper.hpp"

#if MF_OS_IS_WINDOWS
#   define WIN32_LEAN_AND_MEAN 1

#   include <Windows.h>

#else
#   include <sys/stat.h>
#   include <unistd.h>
#endif

namespace MF {
    namespace File {
        bool osDeleteFile(File::Filename_t filename) {
#if MF_OS_IS_WINDOWS
            return DeleteFile(filename);
#else
            return !unlink(filename);
#endif
        }

        bool osDeleteFileOrDirectory(File::Filename_t name) {
#if MF_OS_IS_WINDOWS
            return DeleteFile(name) ? true : RemoveDirectory(name);
#else
            return !remove(name);
#endif
        }

        bool osFileExists(File::Filename_t filename) {
#if MF_OS_IS_WINDOWS
            DWORD attr = GetFileAttributes(filename);
            return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
            struct stat s{};
            return !stat(filename, &s);
#endif
        }

        bool osDirectoryExists(File::Filename_t filename) {
#if MF_OS_IS_WINDOWS
            DWORD attr = GetFileAttributes(filename);
            return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
            struct stat s{};
            return !stat(directoryName, &s) & S_ISDIR(s.st_mode);
#endif
        }

        int osReadFileToBuffer(File::Filename_t filename, char *buffer, File::Filesize_t bufferSize) {
#if MF_OS_IS_WINDOWS
            HANDLE fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL, nullptr);
            if (fileHandle == INVALID_HANDLE_VALUE) {
                return -1;
            }

            DWORD numberOfBytesRead;
            bool returnValue = ReadFile(fileHandle, buffer, bufferSize, &numberOfBytesRead, nullptr);
            CloseHandle(fileHandle);

            return returnValue ? static_cast<int>(numberOfBytesRead) : -1;
#else
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                return -1;
            }

            int bytesRead = read(fd, buffer, bufferSize);
            close(fd);
            return bytesRead;
#endif
        }

        Filesize_t osGetFileSize(File::Filename_t filename) {
#if MF_OS_IS_WINDOWS
            HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     nullptr);
            if (file == INVALID_HANDLE_VALUE) return 0;
            LARGE_INTEGER res;
            GetFileSizeEx(file, &res);
            CloseHandle(file);
            return static_cast<File::Filesize_t>(res.QuadPart);
#else
            struct stat t{};
            if (stat(filename, &t)) return 0;
            return static_cast<File::Filesize_t>(t.st_size);
#endif
        }

        bool osCreateDirectory(File::Filename_t directoryName) {
#if MF_OS_IS_WINDOWS
            return CreateDirectory(directoryName, nullptr);
#else
            !mkdir(directoryName, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
        }

        SFilename_t osGetCWD() {
#if MF_OS_IS_WINDOWS
            DWORD nBufferLength = GetCurrentDirectory(0, nullptr);
            auto lpBuffer = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                                          nBufferLength * sizeof(TCHAR)));
            GetCurrentDirectory(nBufferLength, lpBuffer);
            File::SFilename_t returnValue(lpBuffer);
            HeapFree(GetProcessHeap(), 0, lpBuffer);
            return returnValue;
#else
            char* syscall_return;
#   if defined(_GNU_SOURCE)
            syscall_return = get_current_dir_name();
#   else
            syscall_return = static_cast<char *>(malloc(PATH_MAX));
            getcwd(syscall_return, PATH_MAX);
#   endif
            File::SFilename_t to_return(syscall_return);
            free((void*)syscall_return);
            return to_return;
#endif

        }

        void osGetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t> &result) {
#if MF_OS_IS_WINDOWS
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
                    if (wfd.dwFileAttributes != INVALID_FILE_ATTRIBUTES && (wfd.dwFileAttributes &
                                                                            FILE_ATTRIBUTE_DIRECTORY)) {
                        tempFilename = wfd.cFileName;
                        if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                            continue;
                        } else {
                            tempFilename += MAKE_FILE_NAME FILE_SEPARATOR;
                        }
                        result.push_back(tempFilename);
                    } else {
                        result.emplace_back(wfd.cFileName);
                    }
                } while (FindNextFile(hFind, &wfd));
                FindClose(hFind);
            }
#else
            File::SFilename_t tempFilename;
            static File::Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
            static File::Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
            DIR * d;
            dirent * dir_entry;
            d = opendir(directoryName);
            if (d) {
                while ((dir_entry = readdir(d)) != nullptr) {
                    tempFilename = dir_entry->d_name;
                    if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                        continue;
                    } else if (File::IsDir((std::string(directoryName) + tempFilename).c_str())) {
                        tempFilename.append(FILE_SEPARATOR);
                    }
                    result.emplace_back(tempFilename);
                }
                closedir(d);
            }
#endif
        }

#if MF_OS_IS_WINDOWS
#else
#endif

    }
}
