//
// Created by MartinF on 31/07/2022.
//

//
// Created by MartinF on 03/07/2022.
//

#include <cstring>

#include "MF/SystemErrors.hpp"

#if _GNU_SOURCE
#    define HAS_STRERROR_NP 1
#else
#    define HAS_STRERROR_NP 0
#endif

namespace MF
{
    namespace SystemErrors
    {
        namespace Errno
        {
            static std::string getErrorMessageLocalizedForErrorCode(ErrorCode_t errorCode) {
                // TODO thread-safe with _r or _l
                return strerror(errorCode);
            }

            static std::string getErrorMessageNotLocalizedForErrorCode(ErrorCode_t errorCode) {
                // Weird but it's not available on all platforms,
                // even with the feature test macro.
                // https://www.gnu.org/software/gnulib/manual/html_node/strerrordesc_005fnp.html
                // TODO investigate
#if HAS_STRERROR_NP && 0
                return strerrordesc_np(errorCode);
#else
                return getErrorMessageLocalizedForErrorCode(errorCode);
#endif
            }

            ErrorCode_t getCurrentErrorCode() {
                return errno;
            }

            void setCurrentErrorCode(ErrorCode_t value) {
                errno = value;
            }

            std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode) {
                std::string errorMessage = getSystemErrorMessagesLocalized()
                                               ? getErrorMessageLocalizedForErrorCode(errorCode)
                                               : getErrorMessageNotLocalizedForErrorCode(errorCode);

                std::error_code errorCodeObject(errorCode, std::generic_category());
                return std::system_error(errorCodeObject, errorMessage);
            }
        } // namespace Errno
    } // namespace SystemErrors
} // namespace MF
