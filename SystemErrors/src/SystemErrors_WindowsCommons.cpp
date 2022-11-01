//
// Created by MartinF on 31/07/2022.
//

#if MF_WINDOWS

#    include "SystemErrors_WindowsCommons.hpp"

#    include <sstream>

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Windows.hpp"

namespace MF
{
    namespace SystemErrors
    {
        namespace WindowsCommons
        {
            static constexpr auto getLangId() {
                // TODO: ensure that MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) will always work.
                // It is possible that this locale is not installed on a machine,
                // causing the FormatMessage function to fail with ERROR_RESOURCE_LANG_NOT_FOUND.
                return LANG_USER_DEFAULT;
            }

            std::string getErrorMessageForErrorCode(DWORD errorCode) {
                // Source: https://stackoverflow.com/a/17387176/11996851
                LPSTR messageBuffer = nullptr;

                // Ask Win32 to give us the string version of that message ID.
                // The parameters we pass in, tell Win32 to create the buffer that holds the message
                // for us (because we don't yet know how long the message string will be).
                static constexpr DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                               FORMAT_MESSAGE_FROM_SYSTEM |
                                               FORMAT_MESSAGE_IGNORE_INSERTS;
                size_t size = FormatMessageA(
                    flags, NULL, errorCode, getLangId(), (LPSTR)&messageBuffer, 0, NULL);
                if (size == 0) {
                    auto newErrorCode = GetLastError();
                    std::ostringstream oss;
                    oss << std::hex << std::showbase
                        << "Unable to retrieve the error message for error code " << errorCode
                        << " - FormatMessageA failed with error " << newErrorCode << ".";
                    throw std::runtime_error(oss.str());
                }

                // Small boilerplate to ensure we free the buffer
                // if the constructor of 'std::string' throws.
                Windows::LocalMemoryCloser messageBufferSmart(messageBuffer);

                if ((messageBuffer[size - 3] == '.') && (messageBuffer[size - 2] == '\r') &&
                    (messageBuffer[size - 1] == '\n')) {
                    size -= 3;
                }

                return std::string(messageBuffer, size);
            }

            std::string getErrorMessageForErrorCode(int errorCode) {
                return getErrorMessageForErrorCode(DWORD(errorCode));
            }

            SystemError getSystemErrorForErrorCode(DWORD errorCode, Paradigm paradigm) {
                std::string errorMessage = getErrorMessageForErrorCode(errorCode);

                return SystemError(paradigm, errorCode, errorMessage);
            }

            SystemError getSystemErrorForErrorCode(int errorCode, Paradigm paradigm) {
                std::string errorMessage = getErrorMessageForErrorCode(errorCode);

                return SystemError(paradigm, errorCode, errorMessage);
            }
        } // namespace WindowsCommons
    } // namespace SystemErrors
} // namespace MF
#endif
