//
// Created by MartinF on 06/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP

#include <stdexcept>

namespace MF
{
    /** Errors for this module. */
    enum class DateError_e {
        /** No pattern was available to perform the Date<->string conversion. */
        NO_PATTERN,

        /** Struct tm was corrupted. */
        WRONG_STRUCT_TM,

        /** The number of milliseconds was not acceptable (too high?). */
        WRONG_MS,

        /** String could not be retrieved as Date using the given pattern. */
        WRONG_STRING,

        /** The given time_t could not be converted as date (value too high?). */
        WRONG_TIME_T,

        /** Returned by setter functions if the parameter is not in the correct range. */
        WRONG_TIME_DATA
    };

    class DateError : public std::runtime_error {
       private:
        const DateError_e errorValue;

       public:
        DateError(DateError_e enumValue, const std::string& errorText)
            : std::runtime_error(errorText), errorValue(enumValue) {
        }

        const DateError_e& getErrorValue() const {
            return errorValue;
        }
    };
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP
