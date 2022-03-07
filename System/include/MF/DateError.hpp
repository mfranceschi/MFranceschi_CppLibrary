//
// Created by MartinF on 06/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP

#include <stdexcept>

namespace MF {
    // Errors for this module. Don't use the integer values directly.
    enum class DateError_e {
        NO_PATTERN,            // No pattern was available to perform the Date<->string conversion.
        WRONG_STRUCT_TM,    // Struct tm was corrupted.
        WRONG_MS,            // The number of milliseconds was not acceptable (too high?).
        WRONG_STRING,        // String could not be retrieved as Date using the given pattern.
        WRONG_TIME_T,        // The given time_t could not be converted as date (value too high?).
        WRONG_TIME_DATA        // Returned by setter functions if the parameter is not in the correct range.
    };

    class DateError : public std::runtime_error {
    public:
        const DateError_e ERROR_VALUE;

        DateError(DateError_e enumValue, const std::string &errorText) : std::runtime_error(errorText),
                                                                         ERROR_VALUE(enumValue) {}
    };
}

#endif //MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP
