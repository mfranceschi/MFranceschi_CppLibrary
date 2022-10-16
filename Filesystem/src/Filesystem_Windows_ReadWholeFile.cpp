//
// Created by MartinF on 16/10/2022.
//

#include "MF/Filesystem.hpp"
#include "MF/SystemErrors.hpp"
#include "MF/Windows.hpp"

namespace MF
{
    namespace Filesystem
    {

        struct Windows_ReadFileData : public WholeFileData {
            const HANDLE fileHandle = nullptr;
            const HANDLE mappingHandle = nullptr;

            Windows_ReadFileData(
                const char* content, Filesize_t filesize, HANDLE fileHandle, HANDLE mappingHandle)
                : WholeFileData(content, filesize),
                  fileHandle(fileHandle),
                  mappingHandle(mappingHandle) {
            }

            ~Windows_ReadFileData() override {
                UnmapViewOfFile(getContent());
                CloseHandle(mappingHandle);
                CloseHandle(fileHandle);
            }
        };

        std::unique_ptr<const WholeFileData> readWholeFile(const Filename_t& filename) {
            return readWholeFile(MF::Windows::ConvertString(filename.c_str()));
        }

        std::unique_ptr<const WholeFileData> readWholeFile(const WideFilename_t& filename) {
            Filesize_t filesize = getFileSize(filename);

            Windows::HandleCloser fileHandle(CreateFileW(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr));
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(fileHandle.isInvalid());

            Windows::HandleCloser mappingHandle(
                CreateFileMapping(fileHandle.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(mappingHandle.isInvalid());

            auto lastError = MF::SystemErrors::Win32::getCurrentErrorCode();
            if (lastError == ERROR_ALREADY_EXISTS) {
                // Edge case: don't close this hand
                mappingHandle.release();
                throw MF::SystemErrors::Win32::getSystemErrorForErrorCode(lastError);
            }

            LPVOID mappedViewOfFile = MapViewOfFile(mappingHandle.get(), FILE_MAP_READ, 0, 0, 0);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(mappedViewOfFile == nullptr);

            return std::make_unique<const Windows_ReadFileData>(
                static_cast<const char*>(mappedViewOfFile), filesize, fileHandle.release(),
                mappingHandle.release());
        }
    } // namespace Filesystem
} // namespace MF
