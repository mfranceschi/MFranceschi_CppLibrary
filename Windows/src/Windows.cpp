//
// Created by mfran on 13/04/2020.
//

#if MF_WINDOWS

#include "MF/StringSafePlaceHolder.hpp"
#include "MF/Windows.hpp"

#include <Windows.h>
#include <memory>

namespace MF {
    namespace Windows {
        void ShowErrorMessage(const char *functionName) {
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
                    0, nullptr);

            // Display the error message and exit the process

            lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
                                               (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) functionName) + 40) *
                                               sizeof(TCHAR));
            StringCchPrintf((LPTSTR) lpDisplayBuf,
                            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                            TEXT("%s failed with error %d: %s"),
                            functionName, dw, lpMsgBuf);
            MessageBox(nullptr, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

            LocalFree(lpMsgBuf);
            LocalFree(lpDisplayBuf);
            ExitProcess(dw);
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
            MultiByteToWideChar(
                    CP_UTF8, 0, utf8String, -1,
                    wstr.get(), wchars_num);
            return wstr.get();
        }

        void MakeHandleInheritable(void *handle, bool inherit) {
            SetHandleInformation(handle, HANDLE_FLAG_INHERIT, inherit ? HANDLE_FLAG_INHERIT : 0);
        }
    }
}

#endif
