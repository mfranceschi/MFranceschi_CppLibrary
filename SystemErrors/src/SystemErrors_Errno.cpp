//
// Created by MartinF on 31/07/2022.
//

#include <cstring>
#include <string>
#include <vector>

#include "MF/SystemErrors.hpp"

#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
#    define MF_strerror_r_IS_XSI 1
#    define MF_strerror_r_IS_GNU 0
#else
#    define MF_strerror_r_IS_XSI 0
#    define MF_strerror_r_IS_GNU 1
#endif

namespace MF
{
    namespace SystemErrors
    {
        /**
         * Initial length of the buffer for "strlen_s".
         * It is the value used by the glibc implementation of "perror":
         * https://code.woboq.org/userspace/glibc/stdio-common/perror.c.html
         */
        static constexpr std::size_t DEFAULT_BUFFER_SIZE = 1024;

        using ErrorCode_t = Errno::ErrorCode_t;

        static std::string getErrorMessage_Strerror(ErrorCode_t errorCode) {
            return strerror(errorCode);
        }

#if MF_HAS_strerror_localized
#    error strerror_l
        static const locale_t theRawLocale =
            newlocale(LC_MESSAGES_MASK, "en_US.UTF-8", (locale_t)0);

        static std::string getErrorMessage_Strerror_l(ErrorCode_t errorCode, bool localized) {
            return strerror_l(errorCode, localized ? uselocale((locale_t)0) : theRawLocale);
        }
#endif

#if MF_HAS_strerror_secure
        static std::string getErrorMessage_Strerror_s(ErrorCode_t errorCode) {
            std::vector<char> buffer(DEFAULT_BUFFER_SIZE);
            auto result = strerror_s(buffer.data(), buffer.capacity(), errorCode);
            while (result != 0) {
                switch (result) {
                    case ERANGE:
                        buffer.reserve(buffer.capacity() * 2);
                        break;
                    case EINVAL:
                        throw std::runtime_error("Error - strerror_s returned EINVAL.");
                    default:
                        throw std::runtime_error(
                            "Error - strerror_s returned an unexpected value: " +
                            std::to_string(result) + ".");
                }
            }
            return buffer.data();
        }
#endif

#if MF_HAS_strerror_repeatable
        static std::string getErrorMessage_Strerror_r(ErrorCode_t errorCode) {
            std::vector<char> buffer(DEFAULT_BUFFER_SIZE);
            auto result = strerror_r(errorCode, buffer.data(), buffer.capacity());

#    if MF_strerror_r_IS_GNU
            // AFAIK we can't really know if strerror_r failed or not with the GNU version.
            // Assumption: it's unlikely to happen.
            return result;
#    else
            while (result != 0) {
                switch (result) {
                    case ERANGE:
                        buffer.reserve(buffer.capacity() * 2);
                        break;
                    case EINVAL:
                        throw std::runtime_error("Error - strerror_s returned EINVAL.");
                    default:
                        throw std::runtime_error(
                            "Error - strerror_s returned an unexpected value: " +
                            std::to_string(result) + ".");
                }
            }
            return buffer.data();
#    endif
        }
#endif

#if MF_HAS_strerrordesc_np
        static std::string getErrorMessage_Strerrordesc_np(ErrorCode_t errorCode) {
            return strerrordesc_np(errorCode);
        }
#endif

        static std::string getErrorMessageLocalizedForErrorCode(ErrorCode_t errorCode) {
#if MF_HAS_strerror_localized
            return getErrorMessage_Strerror_l(errorCode, true);
#elif MF_HAS_strerror_secure
            return getErrorMessage_Strerror_s(errorCode);
#elif MF_HAS_strerror_repeatable
            return getErrorMessage_Strerror_r(errorCode);
#else
            return getErrorMessage_Strerror(errorCode);
#endif
        }

        static std::string getErrorMessageNotLocalizedForErrorCode(ErrorCode_t errorCode) {
#if MF_HAS_strerrordesc_np
            return getErrorMessage_Strerrordesc_np(errorCode);
#elif MF_HAS_strerror_localized
            return getErrorMessage_Strerror_l(errorCode, false);
#else
            // At this point, we have no tool to help us get the string message
            // that is guaranteed to be in the en-US locale.
            // Fallback to the localized string message to ensure we have a message to return.
            return getErrorMessageLocalizedForErrorCode(errorCode);
#endif
        }

        ErrorCode_t Errno::getCurrentErrorCode() {
            return errno;
        }

        void Errno::setCurrentErrorCode(ErrorCode_t value) {
            errno = value;
        }

        SystemError Errno::getSystemErrorForErrorCode(ErrorCode_t errorCode) {
            std::string errorMessage = getSystemErrorMessagesLocalized()
                                           ? getErrorMessageLocalizedForErrorCode(errorCode)
                                           : getErrorMessageNotLocalizedForErrorCode(errorCode);

            return SystemError(errorCode, errorMessage);
        }
    } // namespace SystemErrors
} // namespace MF
