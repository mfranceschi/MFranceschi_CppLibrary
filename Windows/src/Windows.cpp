//
// Created by mfran on 13/04/2020.
//

#include "MF/Windows.hpp"

#include <memory>

#include "MF/LightWindows.hpp"
#include "MF/StringSafePlaceHolder.hpp"

namespace MF
{
    namespace Windows
    {
        void ShowErrorMessage(const char *functionName) {
            // Adapted from:
            // https://docs.microsoft.com/fr-fr/windows/win32/debug/retrieving-the-last-error-code

            // Retrieve the system error message for the last-error code
            
            LPTSTR errorMessageBuffer = nullptr;
            LPTSTR textToDisplayInMessageBox;
            const DWORD lastError = GetLastError();

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorMessageBuffer,
                0, nullptr);

            // Display the error message and exit the process

            textToDisplayInMessageBox = static_cast<LPTSTR>(LocalAlloc(
                LMEM_ZEROINIT,
                (lstrlen((LPCTSTR)errorMessageBuffer) + lstrlen((LPCTSTR)functionName) + 40) *
                    sizeof(TCHAR)));
            StringCchPrintf(
                textToDisplayInMessageBox, LocalSize(textToDisplayInMessageBox) / sizeof(TCHAR),
                TEXT("%s failed with error %lu: %s"), functionName, lastError, errorMessageBuffer);
            MessageBox(nullptr, (LPCTSTR)textToDisplayInMessageBox, TEXT("Error"), MB_OK);

            LocalFree(errorMessageBuffer);
            LocalFree(textToDisplayInMessageBox);
            ExitProcess(lastError);
        }

        std::system_error GetCurrentSystemError() {
            // Source: https://stackoverflow.com/a/17387176/11996851
            DWORD errorMessageID = ::GetLastError();

            LPSTR messageBuffer = nullptr;

            // Ask Win32 to give us the string version of that message ID.
            // The parameters we pass in, tell Win32 to create the buffer that holds the message for
            // us (because we don't yet know how long the message string will be).
            size_t size = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&messageBuffer, 0, NULL);

            // Copy the error message into a std::string.
            std::string message(messageBuffer, size);

            // Free the Win32's string's buffer.
            LocalFree(messageBuffer);

            return std::system_error(
                std::error_code(errorMessageID, std::generic_category()), message);
        }

        std::wstring ConvertString(const char *utf8String) {
            // Thanks to https://stackoverflow.com/a/6693107 !
            int wchars_num = MultiByteToWideChar(
                CP_UTF8, // Source (and so the expected processing) is UTF-8.
                0, // Because of UTF-8.
                utf8String, // C-string source
                -1, // Indicates that the source C-string is null-terminated.
                nullptr, // No use for now.
                0); // We ask this function to return the number of bytes to allocate.

            std::unique_ptr<wchar_t[]> wstr = std::make_unique<wchar_t[]>(wchars_num);
            MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, wstr.get(), wchars_num);
            return wstr.get();
        }
    } // namespace Windows
} // namespace MF
