//
// Created by MartinF on 06/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP

namespace MF {
    // Errors for this module. Don't use the integer values directly.
    enum class DateError {
        NO_PATTERN,            // No pattern was available to perform the Date<->string conversion.
        WRONG_STRUCT_TM,    // Struct tm was corrupted.
        WRONG_MS,            // The number of milliseconds was not acceptable (too high?).
        WRONG_STRING,        // String could not be retrieved as Date using the given pattern.
        WRONG_TIME_T,        // The given time_t could not be converted as date (value too high?).
        WRONG_TIME_DATA        // Returned by setter functions if the parameter is not in the correct range.
    };
}


#endif //MFRANCESCHI_CPPLIBRARIES_DATEERROR_HPP
