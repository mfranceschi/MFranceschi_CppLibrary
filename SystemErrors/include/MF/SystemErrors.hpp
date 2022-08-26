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
        enum class Paradigm { ERRNO, WIN32, WSA };

        class SystemError : public std::runtime_error {
           public:
            SystemError(long errorCode, const std::string& message)
                : std::runtime_error(message), errorCode(errorCode) {
            }
            SystemError(Paradigm paradigm, long errorCode, const std::string& message)
                : std::runtime_error(message), errorCode(errorCode), paradigm(paradigm) {
            }
            long getErrorCode() const {
                return errorCode;
            }
            Paradigm getParadigm() const {
                return paradigm;
            }

           private:
            const long errorCode = 0;
            const Paradigm paradigm = Paradigm::ERRNO;
        };

        struct Errno {
            using ErrorCode_t = int;

            static ErrorCode_t getCurrentErrorCode();
            static void setCurrentErrorCode(ErrorCode_t value);

            static SystemError getSystemErrorForErrorCode(ErrorCode_t errorCode);

            static inline SystemError getCurrentSystemError() {
                ErrorCode_t errorCode = getCurrentErrorCode();
                return getSystemErrorForErrorCode(errorCode);
            }

            static inline void throwCurrentSystemErrorIf(bool condition) {
                if (condition) {
                    throw getCurrentSystemError();
                }
            }
        };

#if MF_WINDOWS
        struct Win32 {
            using ErrorCode_t = unsigned long; // equivalent to DWORD

            static ErrorCode_t getCurrentErrorCode();
            static void setCurrentErrorCode(ErrorCode_t value);

            static SystemError getSystemErrorForErrorCode(ErrorCode_t errorCode);

            static inline SystemError getCurrentSystemError() {
                ErrorCode_t errorCode = getCurrentErrorCode();
                return getSystemErrorForErrorCode(errorCode);
            }

            static inline void throwCurrentSystemErrorIf(bool condition) {
                if (condition) {
                    throw getCurrentSystemError();
                }
            }
        };

        struct Wsa {
            using ErrorCode_t = int;

            static ErrorCode_t getCurrentErrorCode();
            static void setCurrentErrorCode(ErrorCode_t value);

            static SystemError getSystemErrorForErrorCode(ErrorCode_t errorCode);

            static inline SystemError getCurrentSystemError() {
                ErrorCode_t errorCode = getCurrentErrorCode();
                return getSystemErrorForErrorCode(errorCode);
            }

            static inline void throwCurrentSystemErrorIf(bool condition) {
                if (condition) {
                    throw getCurrentSystemError();
                }
            }
        };
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
