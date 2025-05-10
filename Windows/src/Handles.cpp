//
// Created by MartinF on 06/05/2023.
//

#include "MF/LightWindows.hpp"
#include "MF/SystemErrors.hpp"
#include "MF/Windows.hpp"

namespace MF
{
    namespace Windows
    {
        namespace Handles
        {
            void makeHandleInheritable(HANDLE handle, bool yesOrNo) {
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    SetHandleInformation(
                        handle, HANDLE_FLAG_INHERIT, yesOrNo ? HANDLE_FLAG_INHERIT : 0) == FALSE);
            }

            bool isHandleInheritable(HANDLE handle) {
                DWORD flags;
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    GetHandleInformation(handle, &flags) == FALSE);
                return flags & HANDLE_FLAG_INHERIT;
            }
        } // namespace Handles
    } // namespace Windows
} // namespace MF
