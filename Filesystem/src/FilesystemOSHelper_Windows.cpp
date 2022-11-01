//
// Created by Utilisateur on 22/03/2022.
//

//
// Created by MartinF on 05/03/2022.
//

#if MF_WINDOWS

#    include "FilesystemOSHelper.hpp"
#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Windows.hpp"

namespace MF
{
    namespace Filesystem
    {
        static void readBytesFromFileHandle(
            const Windows::HandleCloser &handleCloser, char *buffer, Filesize_t bufferSize) {
            DWORD numberOfBytesRead = 0;
            BOOL returnValue =
                ReadFile(handleCloser.get(), buffer, bufferSize, &numberOfBytesRead, nullptr);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(returnValue == FALSE);
        }

        void osReadFileToBuffer(const Filename_t &filename, char *buffer, Filesize_t bufferSize) {
            if (bufferSize == 0) {
                throw std::invalid_argument("Tried to read a buffer of size 0.");
            }

            Windows::HandleCloser handle(CreateFileA(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle.isInvalid());

            readBytesFromFileHandle(handle, buffer, bufferSize);
        }

        void osReadFileToBuffer(
            const WideFilename_t &filename, char *buffer, Filesize_t bufferSize) {
            if (bufferSize == 0) {
                throw std::invalid_argument("Tried to read a buffer of size 0.");
            }

            Windows::HandleCloser handle(CreateFileW(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle.isInvalid());

            readBytesFromFileHandle(handle, buffer, bufferSize);
        }

        static bool isCurrentOrParentDir(const Filename_t &string) {
            static Filename_t CURRENT_DIR = ".";
            static Filename_t PARENT_DIR = "..";

            return string == CURRENT_DIR || string == PARENT_DIR;
        }

        static bool isCurrentOrParentDir(const WideFilename_t &string) {
            static WideFilename_t CURRENT_DIR = L".";
            static WideFilename_t PARENT_DIR = L"..";

            return string == CURRENT_DIR || string == PARENT_DIR;
        }

        void osGetDirectoryContents(
            const Filename_t &directoryName, std::vector<Filename_t> &result) {
            Filename_t tempFolderName = directoryName + "*";

            WIN32_FIND_DATAA wfd;

            Windows::FileHandleCloser handle(FindFirstFileA(tempFolderName.c_str(), &wfd));

            if (handle.isInvalid()) {
                auto errorCode = SystemErrors::Win32::getCurrentErrorCode();
                if (errorCode == ERROR_FILE_NOT_FOUND) {
                    return;
                }
                throw SystemErrors::Win32::getSystemErrorForErrorCode(errorCode);
            }

            do {
                // If it is a directory, then remove "." and ".." or append an ending backslash.
                Windows::FileAttributes fileAttributes(wfd.dwFileAttributes);
                Filename_t filename = wfd.cFileName;
                if (fileAttributes.isValid() && fileAttributes.isDirectory()) {
                    if (isCurrentOrParentDir(filename)) {
                        continue;
                    }

                    result.push_back(filename + FILE_SEPARATOR);
                } else {
                    result.emplace_back(filename);
                }
            } while (FindNextFileA(handle.get(), &wfd) != 0);
        }

        void osGetDirectoryContents(
            const WideFilename_t &directoryName, std::vector<WideFilename_t> &result) {
            WideFilename_t tempFolderName = directoryName + L"*";

            WIN32_FIND_DATAW wfd;

            Windows::FileHandleCloser handle(FindFirstFileW(tempFolderName.c_str(), &wfd));

            if (handle.isInvalid()) {
                auto errorCode = SystemErrors::Win32::getCurrentErrorCode();
                if (errorCode == ERROR_FILE_NOT_FOUND) {
                    return;
                }
                throw SystemErrors::Win32::getSystemErrorForErrorCode(errorCode);
            }

            do {
                // If it is a directory, then remove "." and ".." or append an ending backslash.
                Windows::FileAttributes fileAttributes(wfd.dwFileAttributes);
                WideFilename_t filename = wfd.cFileName;
                if (fileAttributes.isValid() && fileAttributes.isDirectory()) {
                    if (isCurrentOrParentDir(filename)) {
                        continue;
                    }

                    result.push_back(filename + FILE_SEPARATOR_WIDE);
                } else {
                    result.emplace_back(filename);
                }
            } while (FindNextFileW(handle.get(), &wfd) != 0);
        }

    } // namespace Filesystem
} // namespace MF

#endif
