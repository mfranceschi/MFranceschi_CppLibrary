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
#if MF_WINDOWS
        using ErrorCode_t = std::uint32_t;
#else
        using ErrorCode_t = int;
#endif
        ErrorCode_t getCurrentErrorCode();

        std::system_error getCurrentSystemError();

        std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode);

        /** Shorthand utility. */
        inline void throwCurrentSystemErrorIf(bool condition) {
            if (condition) {
                throw getCurrentSystemError();
            }
        }

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
