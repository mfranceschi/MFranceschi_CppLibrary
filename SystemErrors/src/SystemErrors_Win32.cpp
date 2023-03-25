//
// Created by MartinF on 31/07/2022.
//

#if MF_WINDOWS

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "SystemErrors_WindowsCommons.hpp"

namespace MF
{
    namespace SystemErrors
    {
        using ErrorCode_t = Win32::ErrorCode_t;

        static_assert(std::is_same<DWORD, unsigned long>::value, "DWORD is not unsigned long!");
        static_assert(
            std::is_same<DWORD, ErrorCode_t>::value,
            "ErrorCode_t of Win32 MUST be identical to a DWORD!");

        ErrorCode_t Win32::getCurrentErrorCode() {
            return GetLastError();
        }

        void Win32::setCurrentErrorCode(ErrorCode_t value) {
            SetLastError(value);
        }

        SystemError Win32::getSystemErrorForErrorCode(ErrorCode_t errorCode) {
            return WindowsCommons::getSystemErrorForErrorCode(errorCode, Paradigm::Win32);
        }
    } // namespace SystemErrors
} // namespace MF
#endif
