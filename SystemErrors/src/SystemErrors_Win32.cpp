//
// Created by MartinF on 31/07/2022.
//

//
// Created by MartinF on 03/07/2022.
//

#if MF_WINDOWS

#    include <memory>

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace SystemErrors
    {
        namespace Win32
        {
            static constexpr auto getLangId(bool localized) {
                return MAKELANGID(LANG_NEUTRAL, localized ? SUBLANG_DEFAULT : SUBLANG_NEUTRAL);
            }

            ErrorCode_t getCurrentErrorCode() {
                return GetLastError();
            }

            void setCurrentErrorCode(ErrorCode_t value) {
                SetLastError(value);
            }

            static std::string getErrorMessageForErrorCode(ErrorCode_t errorCode, bool localized) {
                // Source: https://stackoverflow.com/a/17387176/11996851
                LPSTR messageBuffer = nullptr;

                // Ask Win32 to give us the string version of that message ID.
                // The parameters we pass in, tell Win32 to create the buffer that holds the message
                // for us (because we don't yet know how long the message string will be).
                size_t size = FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, errorCode, getLangId(localized), (LPSTR)&messageBuffer, 0, NULL);

                // Small boilerplate to ensure we free the buffer
                // if the constructor of 'std::string' throws.
                std::unique_ptr<char, decltype(&LocalFree)> messageBufferSmart(
                    messageBuffer, &LocalFree);

                return std::string(messageBuffer, size);
            }

            std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode) {
                std::string errorMessage =
                    getErrorMessageForErrorCode(errorCode, getSystemErrorMessagesLocalized());

                std::error_code errorCodeObject(
                    static_cast<int>(errorCode), std::generic_category());
                return std::system_error(errorCodeObject, errorMessage);
            }
        } // namespace Win32
    } // namespace SystemErrors
} // namespace MF
#endif