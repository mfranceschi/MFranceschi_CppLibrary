//
// Created by Utilisateur on 22/03/2022.
//

//
// Created by MartinF on 05/03/2022.
//

#if MF_WINDOWS

#    include <cassert>
#    include <cstring>
#    include <functional>

#    include "FilesystemOSHelper.hpp"
#    include "MF/LightWindows.hpp"
#    include "MF/Windows.hpp"

namespace MF
{
    namespace Filesystem
    {
        namespace
        {
            template <typename ResourceType, typename Closer>
            class ResourceCloser {
               public:
                ResourceCloser(ResourceType resource, Closer closer)
                    : resource(resource), closer(closer) {
                }

                const ResourceType &get() const {
                    return resource;
                }

                virtual ~ResourceCloser() {
                    closer(resource);
                }

               private:
                Closer closer;
                ResourceType resource;
            };

            class HandleCloser : public ResourceCloser<HANDLE, decltype(&CloseHandle)> {
               public:
                HandleCloser(HANDLE handle)
                    : ResourceCloser<HANDLE, decltype(&CloseHandle)>(handle, CloseHandle) {
                }

                bool isInvalid() const {
                    return get() == INVALID_HANDLE_VALUE;
                }
            };

            class FileHandleCloser : public ResourceCloser<HANDLE, decltype(&FindClose)> {
               public:
                FileHandleCloser(HANDLE handle)
                    : ResourceCloser<HANDLE, decltype(&FindClose)>(handle, FindClose) {
                }

                bool isInvalid() const {
                    return get() == INVALID_HANDLE_VALUE;
                }
            };
        } // namespace

        bool osDeleteFile(Filename_t filename) {
            return DeleteFile(filename);
        }

        bool osDeleteFileOrDirectory(Filename_t name) {
            return DeleteFile(name) != 0 ? true : RemoveDirectory(name);
        }

        bool osDeleteDirectory(Filename_t name) {
            return RemoveDirectory(name);
        }

        bool osFileExists(Filename_t filename) {
            Windows::FileAttributes attr(GetFileAttributes(filename));
            return attr.isValid() && attr.isFile();
        }

        bool osDirectoryExists(Filename_t filename) {
            Windows::FileAttributes attr(GetFileAttributes(filename));
            return attr.isValid() && attr.isDirectory();
        }

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize) {
            HandleCloser handle(CreateFile(
                filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
            if (handle.isInvalid()) {
                return -1;
            }

            DWORD numberOfBytesRead = 0;
            BOOL returnValue =
                ReadFile(handle.get(), buffer, bufferSize, &numberOfBytesRead, nullptr);

            return returnValue == 0 ? -1 : static_cast<int>(numberOfBytesRead);
        }

        Filesize_t osGetFileSize(Filename_t filename) {
            HandleCloser handle(CreateFile(
                filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
            if (handle.isInvalid()) {
                return 0;
            }

            LARGE_INTEGER res;
            GetFileSizeEx(handle.get(), &res);
            return static_cast<Filesize_t>(res.QuadPart);
        }

        bool osCreateDirectory(Filename_t directoryName) {
            return CreateDirectory(directoryName, nullptr);
        }

        SFilename_t osGetCWD() {
            DWORD bufferLength = GetCurrentDirectory(0, nullptr);
            assert(bufferLength != 0);

            std::vector<TCHAR> buffer(bufferLength);
            GetCurrentDirectory(bufferLength, buffer.data());

            return buffer.data();
        }

        void osGetDirectoryContents(
            const SFilename_t &directoryName, std::vector<SFilename_t> &result) {
            static Filename_t CURRENT_DIR = MAKE_FILE_NAME ".";
            static Filename_t PARENT_DIR = MAKE_FILE_NAME "..";

            SFilename_t tempFolderName = directoryName + MAKE_FILE_NAME "*";

            WIN32_FIND_DATA wfd;

            FileHandleCloser handle(FindFirstFile(tempFolderName.c_str(), &wfd));

            if (!handle.isInvalid()) {
                do {
                    // If it is a directory, then remove "." and ".." or append an ending backslash.
                    Windows::FileAttributes fileAttributes(wfd.dwFileAttributes);
                    Filename_t filename = wfd.cFileName;
                    if (fileAttributes.isValid() && fileAttributes.isDirectory()) {
                        if (std::strcmp(filename, CURRENT_DIR) == 0 ||
                            std::strcmp(filename, PARENT_DIR) == 0) {
                            // Ignore "." and ".."
                            continue;
                        }

                        result.push_back(filename + SFilename_t(MAKE_FILE_NAME FILE_SEPARATOR));
                    } else {
                        result.emplace_back(filename);
                    }
                } while (FindNextFile(handle.get(), &wfd));
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

            rfd->contents = static_cast<const char *>(
                MapViewOfFile(rfd->mappingHandle, FILE_MAP_READ, 0, 0, 0));
            if (rfd->contents == nullptr) {
                CloseHandle(rfd->mappingHandle);
                CloseHandle(rfd->fileHandle);
                return nullptr;
            }

            return rfd;
        }

    } // namespace Filesystem
} // namespace MF

#endif
