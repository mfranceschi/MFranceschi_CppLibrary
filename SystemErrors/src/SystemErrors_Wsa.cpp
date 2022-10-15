//
// Created by MartinF on 31/07/2022.
//

#if MF_WINDOWS && MF_SystemErrors_WSA

#    include <WinSock2.h>

#    include "MF/SystemErrors.hpp"
#    include "SystemErrors_WindowsCommons.hpp"

#    pragma comment(lib, "Ws2_32.lib")

namespace MF
{
    namespace SystemErrors
    {
        using ErrorCode_t = Wsa::ErrorCode_t;

        ErrorCode_t Wsa::getCurrentErrorCode() {
            return WSAGetLastError();
        }

        void Wsa::setCurrentErrorCode(ErrorCode_t value) {
            WSASetLastError(value);
        }

        SystemError Wsa::getSystemErrorForErrorCode(ErrorCode_t errorCode) {
            return WindowsCommons::getSystemErrorForErrorCode(errorCode, Paradigm::Wsa);
        }
    } // namespace SystemErrors
} // namespace MF
#endif
