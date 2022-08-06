//
// Created by MartinF on 31/07/2022.
//

#include <cstring>

#include "MF/SystemErrors.hpp"

namespace MF
{
    namespace SystemErrors
    {
        using ErrorCode_t = Errno::ErrorCode_t;

        static std::string getErrorMessageLocalizedForErrorCode(ErrorCode_t errorCode) {
            // TODO thread-safe with _r or _l
            return strerror(errorCode);
        }

        static std::string getErrorMessageNotLocalizedForErrorCode(ErrorCode_t errorCode) {
#if MF_HAS_strerrordesc_np
            // For this function, just doing the feature test macro is not enough.
            // It's not available on all platforms, and I couldn't find a way to determine
            // whether it can be used or not.
            // https://www.gnu.org/software/gnulib/manual/html_node/strerrordesc_005fnp.html
            return strerrordesc_np(errorCode);
#else
            return getErrorMessageLocalizedForErrorCode(errorCode);
#endif
        }

        ErrorCode_t Errno::getCurrentErrorCode() {
            return errno;
        }

        void Errno::setCurrentErrorCode(ErrorCode_t value) {
            errno = value;
        }

        std::system_error Errno::getSystemErrorForErrorCode(ErrorCode_t errorCode) {
            std::string errorMessage = getSystemErrorMessagesLocalized()
                                           ? getErrorMessageLocalizedForErrorCode(errorCode)
                                           : getErrorMessageNotLocalizedForErrorCode(errorCode);

            std::error_code errorCodeObject(errorCode, std::generic_category());
            return std::system_error(errorCodeObject, errorMessage);
        }
    } // namespace SystemErrors
} // namespace MF
