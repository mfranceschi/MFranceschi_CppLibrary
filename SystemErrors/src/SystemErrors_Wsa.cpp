//
// Created by MartinF on 31/07/2022.
//

//
// Created by MartinF on 03/07/2022.
//

#if MF_WINDOWS && MF_SystemErrors_WSA

#    include "MF/SystemErrors.hpp"
#    include "SystemErrors_WindowsCommons.hpp"
#    include "WinSock2.h"

#    pragma comment(lib, "Ws2_32.lib")

namespace MF
{
    namespace SystemErrors
    {
        namespace Wsa
        {
            ErrorCode_t getCurrentErrorCode() {
                return WSAGetLastError();
            }

            void setCurrentErrorCode(ErrorCode_t value) {
                WSASetLastError(value);
            }

            std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode) {
                return WindowsCommons::getSystemErrorForErrorCode(errorCode);
            }
        } // namespace Wsa
    } // namespace SystemErrors
} // namespace MF
#endif
