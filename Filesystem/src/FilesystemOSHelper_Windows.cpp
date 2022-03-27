//
// Created by Utilisateur on 22/03/2022.
//

//
// Created by MartinF on 05/03/2022.
//

#if MF_WINDOWS

#    include <cassert>

#    include "FilesystemOSHelper.hpp"
#    include "MF/LightWindows.hpp"

namespace MF
{
    namespace Filesystem
    {
        bool osDeleteFile(Filename_t filename) {
            return DeleteFile(filename);
        }

        bool osDeleteFileOrDirectory(Filename_t name) {
            return DeleteFile(name) ? true : RemoveDirectory(name);
        }

        bool osFileExists(Filename_t filename) {
            DWORD attr = GetFileAttributes(filename);
            return !(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
        }

        bool osDirectoryExists(Filename_t filename) {
            DWORD attr = GetFileAttributes(filename);
            return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
        }

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize) {
            HANDLE fileHandle = CreateFile(
                filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr);
            if (fileHandle == INVALID_HANDLE_VALUE) {
                return -1;
            }

            DWORD numberOfBytesRead;
            bool returnValue =
                ReadFile(fileHandle, buffer, bufferSize, &numberOfBytesRead, nullptr);
            CloseHandle(fileHandle);

            return returnValue ? static_cast<int>(numberOfBytesRead) : -1;
        }

        Filesize_t osGetFileSize(Filename_t filename) {
            HANDLE file = CreateFile(
                filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE) return 0;
            LARGE_INTEGER res;
            GetFileSizeEx(file, &res);
            CloseHandle(file);
            return static_cast<Filesize_t>(res.QuadPart);
        }

        bool osCreateDirectory(Filename_t directoryName) {
            return CreateDirectory(directoryName, nullptr);
        }

        SFilename_t osGetCWD() {
            DWORD nBufferLength = GetCurrentDirectory(0, nullptr);
            auto lpBuffer = static_cast<LPTSTR>(
                HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nBufferLength * sizeof(TCHAR)));
            GetCurrentDirectory(nBufferLength, lpBuffer);
            SFilename_t returnValue(lpBuffer);
            HeapFree(GetProcessHeap(), 0, lpBuffer);
            return returnValue;
        }

        void osGetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result) {
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
                    if (wfd.dwFileAttributes != INVALID_FILE_ATTRIBUTES &&
                        (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
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
        }

        struct Windows_ReadFileData_Dummy : public WholeFileData {
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

        std::unique_ptr<const WholeFileData> osReadWholeFile(Filename_t filename) {
            auto rfd = std::make_unique<Windows_ReadFileData_Dummy>();

            rfd->size = osGetFileSize(filename);
            if (rfd->size == 0) {
                return nullptr;
            }

            rfd->fileHandle = CreateFile(
                filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr);
            if (rfd->fileHandle == INVALID_HANDLE_VALUE) {
                return nullptr;
            }

            rfd->mappingHandle =
                CreateFileMapping(rfd->fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (rfd->mappingHandle == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
                CloseHandle(rfd->fileHandle);
                return nullptr;
            }

            rfd->contents = (const char *)MapViewOfFile(rfd->mappingHandle, FILE_MAP_READ, 0, 0, 0);
            if (rfd->contents == nullptr) {
                CloseHandle(rfd->mappingHandle);
                CloseHandle(rfd->fileHandle);
                return nullptr;
            }

            return rfd;
        }

        void osCloseReadFileData(const WholeFileData *readFileData1) {
            const auto *readFileData = dynamic_cast<const osReadFileData_t *>(readFileData1);
            assert(readFileData);

            UnmapViewOfFile(readFileData->contents);
            CloseHandle(readFileData->mappingHandle);
            CloseHandle(readFileData->fileHandle);
            delete readFileData;
        }

    } // namespace Filesystem
} // namespace MF

#endif
