//
// Created by MartinF on 03/07/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_HPP
#define MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_HPP

#include <system_error>

namespace MF
{
    namespace SystemErrors
    {
        /*
#if MF_WINDOWS
        using ErrorCode_t = std::uint32_t;
#else
        using ErrorCode_t = int;
#endif
        ErrorCode_t getCurrentErrorCode();

        std::system_error getCurrentSystemError();

        std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode);

        inline void throwCurrentSystemErrorIf(bool condition) {
            if (condition) {
                throw getCurrentSystemError();
            }
        }
        */

        namespace Errno
        {
            using ErrorCode_t = int;

            ErrorCode_t getCurrentErrorCode();
            void setCurrentErrorCode(ErrorCode_t value);

            std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode);

            inline std::system_error getCurrentSystemError() {
                ErrorCode_t errorCode = getCurrentErrorCode();
                return getSystemErrorForErrorCode(errorCode);
            }

            inline void throwCurrentSystemErrorIf(bool condition) {
                if (condition) {
                    throw getCurrentSystemError();
                }
            }
        } // namespace Errno

#if MF_WINDOWS
        namespace Win32
        {
            using ErrorCode_t = std::uint32_t; // equivalent to DWORD

            ErrorCode_t getCurrentErrorCode();
            void setCurrentErrorCode(ErrorCode_t value);

            std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode);

            inline std::system_error getCurrentSystemError() {
                ErrorCode_t errorCode = getCurrentErrorCode();
                return getSystemErrorForErrorCode(errorCode);
            }

            inline void throwCurrentSystemErrorIf(bool condition) {
                if (condition) {
                    throw getCurrentSystemError();
                }
            }
        } // namespace Win32
#endif

        /**
         * Sets the value of the internal flag 'systemErrorMessagesLocalized'.
         * If 'true' then error description strings are localized.
         * Default value is 'true'.
         *
         * Please note that that behaviour is not guaranteed.
         */
        void setSystemErrorMessagesLocalized(bool newValue);

        /**
         * Returns the value of the internal flag 'systemErrorMessagesLocalized'.
         * If 'true' then error description strings are localized.
         * Default value is 'true'.
         *
         * Please note that that behaviour is not guaranteed.
         */
        bool getSystemErrorMessagesLocalized();
    } // namespace SystemErrors
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_HPP
