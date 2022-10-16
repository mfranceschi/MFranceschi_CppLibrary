//
// Created by MartinF on 15/10/2022.
//

#if MF_WINDOWS

#    include "MF/Filesystem.hpp"
#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Windows.hpp"

#    undef DeleteFile
#    undef CreateDirectory

using namespace MF::SystemErrors;

namespace MF
{
    namespace Filesystem
    {
        namespace internals
        {
            static Filesize_t getFileSizeFromHandle(HANDLE handle) {
                Windows::HandleCloser handleCloser(handle);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handleCloser.isInvalid());

                LARGE_INTEGER res;
                BOOL success = GetFileSizeEx(handleCloser.get(), &res);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(success == 0);

                return static_cast<Filesize_t>(res.QuadPart);
            }
        } // namespace internals

        void deleteFile(const Filename_t& filename) {
            BOOL success = DeleteFileA(filename.c_str());
            Win32::throwCurrentSystemErrorIf(success == 0);
        }

        void deleteFile(const WideFilename_t& filename) {
            BOOL success = DeleteFileW(filename.c_str());
            Win32::throwCurrentSystemErrorIf(success == 0);
        }

        void deleteDirectory(const Filename_t& filename) {
            BOOL success = RemoveDirectoryA(filename.c_str());
            Win32::throwCurrentSystemErrorIf(success == 0);
        }

        void deleteDirectory(const WideFilename_t& filename) {
            BOOL success = RemoveDirectoryW(filename.c_str());
            Win32::throwCurrentSystemErrorIf(success == 0);
        }

        bool isFile(const Filename_t& filename) {
            Windows::FileAttributes attr(GetFileAttributesA(filename.c_str()));
            return attr.isValid() && attr.isFile();
        }

        bool isFile(const WideFilename_t& filename) {
            Windows::FileAttributes attr(GetFileAttributesW(filename.c_str()));
            return attr.isValid() && attr.isFile();
        }

        bool isDir(const Filename_t& filename) {
            Windows::FileAttributes attr(GetFileAttributesA(filename.c_str()));
            return attr.isValid() && attr.isDirectory();
        }

        bool isDir(const WideFilename_t& filename) {
            Windows::FileAttributes attr(GetFileAttributesW(filename.c_str()));
            return attr.isValid() && attr.isDirectory();
        }

        Filesize_t getFileSize(const Filename_t& filename) {
            return internals::getFileSizeFromHandle(CreateFileA(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
        }

        Filesize_t getFileSize(const WideFilename_t& filename) {
            return internals::getFileSizeFromHandle(CreateFileW(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
        }

        Filename_t getCWD() {
            DWORD bufferLength = GetCurrentDirectoryA(0, nullptr);
            Win32::throwCurrentSystemErrorIf(bufferLength == 0);

            std::vector<char> buffer(bufferLength);
            GetCurrentDirectoryA(bufferLength, buffer.data());

            return buffer.data();
        }

        WideFilename_t getCWDWide() {
            DWORD bufferLength = GetCurrentDirectoryW(0, nullptr);
            Win32::throwCurrentSystemErrorIf(bufferLength == 0);

            std::vector<wchar_t> buffer(bufferLength);
            GetCurrentDirectoryW(bufferLength, buffer.data());

            return buffer.data();
        }

        void createDirectory(const Filename_t& filename) {
            BOOL success = CreateDirectoryA(filename.c_str(), nullptr);
            Win32::throwCurrentSystemErrorIf(success == 0);
        }

        void createDirectory(const WideFilename_t& filename) {
            BOOL success = CreateDirectoryW(filename.c_str(), nullptr);
            Win32::throwCurrentSystemErrorIf(success == 0);
        }
    } // namespace Filesystem
} // namespace MF
#endif
