//
// Created by MartinF on 05/03/2022.
//

#include <cassert>

#include "FilesystemOSHelper.hpp"

#if MF_OS_IS_WINDOWS
#   define WIN32_LEAN_AND_MEAN 1

#   include <Windows.h>

#else
#   include <sys/stat.h>
#   include <unistd.h>
#endif

namespace MF {
    namespace Filesystem {
        bool osDeleteFile(Filename_t filename) {
#if MF_OS_IS_WINDOWS
            return DeleteFile(filename);
#else
            return !unlink(filename);
#endif
        }

        bool osDeleteFileOrDirectory(Filename_t name) {
#if MF_OS_IS_WINDOWS
            return DeleteFile(name) ? true : RemoveDirectory(name);
#else
            return !remove(name);
#endif
        }

        bool osFileExists(Filename_t filename) {
#if MF_OS_IS_WINDOWS
            DWORD attr = GetFileAttributes(filename);
            return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
            struct stat s{};
            return !stat(filename, &s);
#endif
        }

        bool osDirectoryExists(Filename_t filename) {
#if MF_OS_IS_WINDOWS
            DWORD attr = GetFileAttributes(filename);
            return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
            struct stat s{};
            return !stat(directoryName, &s) & S_ISDIR(s.st_mode);
#endif
        }

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize) {
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

        Filesize_t osGetFileSize(Filename_t filename) {
#if MF_OS_IS_WINDOWS
            HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     nullptr);
            if (file == INVALID_HANDLE_VALUE) return 0;
            LARGE_INTEGER res;
            GetFileSizeEx(file, &res);
            CloseHandle(file);
            return static_cast<Filesize_t>(res.QuadPart);
#else
            struct stat t{};
            if (stat(filename, &t)) return 0;
            return static_cast<File::Filesize_t>(t.st_size);
#endif
        }

        bool osCreateDirectory(Filename_t directoryName) {
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
            SFilename_t returnValue(lpBuffer);
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

        void osGetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result) {
#if MF_OS_IS_WINDOWS
            SFilename_t tempFilename;
            static Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
            static Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
            WIN32_FIND_DATA wfd;
            HANDLE hFind;
            SFilename_t tempFolderName = directoryName;
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
            static Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
            static Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
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

        struct Windows_ReadFileData_Dummy : public ReadFileData {
            HANDLE fileHandle = nullptr;
            HANDLE mappingHandle = nullptr;
        };

        struct Windows_ReadFileData : public Windows_ReadFileData_Dummy {
            ~Windows_ReadFileData() override {
                UnmapViewOfFile(contents);
                CloseHandle(mappingHandle);
                CloseHandle(fileHandle);
            }
        };

        using osReadFileData_t = Windows_ReadFileData;
#else
        struct Unix_ReadFileData : public File::ReadFileData {
            int fd = -1;
        };
        using osReadFileData_t = Unix_ReadFileData;
#endif

        std::unique_ptr<const ReadFileData> osOpenFile(Filename_t filename) {
#if MF_OS_IS_WINDOWS
            auto rfd = std::make_unique<Windows_ReadFileData_Dummy>();

            rfd->size = osGetFileSize(filename);
            if (rfd->size == 0) {
                return nullptr;
            }

            rfd->fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL, nullptr);
            if (rfd->fileHandle == INVALID_HANDLE_VALUE) {
                return nullptr;
            }

            rfd->mappingHandle = CreateFileMapping(rfd->fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (rfd->mappingHandle == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
                CloseHandle(rfd->fileHandle);
                return nullptr;
            }

            rfd->contents = (const char *) MapViewOfFile(rfd->mappingHandle, FILE_MAP_READ, 0, 0, 0);
            if (rfd->contents == nullptr) {
                CloseHandle(rfd->mappingHandle);
                CloseHandle(rfd->fileHandle);
                return nullptr;
            }

            return rfd;
#else
            auto rfd = new Unix_ReadFileData;

            if ((rfd->fd = open(filename, O_RDONLY)) == -1) {
                delete rfd;
                return nullptr;
            }

            struct stat st;
            if (fstat(rfd->fd, &st) == 0) {
                rfd->size = st.st_size;
            } else {
                delete rfd;
                return nullptr;
            }

            rfd->contents = (const char*)mmap(nullptr, rfd->size, PROT_READ, MAP_PRIVATE, rfd->fd, 0);
            if (rfd->contents == MAP_FAILED) {
                delete rfd;
                return nullptr;
            }

            return rfd;
#endif
        }

        void osCloseReadFileData(const ReadFileData *readFileData1) {
            const auto *readFileData = dynamic_cast<const osReadFileData_t *>(readFileData1);
            assert(readFileData);

#if MF_OS_IS_WINDOWS
            UnmapViewOfFile(readFileData->contents);
            CloseHandle(readFileData->mappingHandle);
            CloseHandle(readFileData->fileHandle);
            delete readFileData;
#else
            close(readFileData->fd);
            delete readFileData;
#endif
        }

#if MF_OS_IS_WINDOWS
#else
#endif

    }
}
