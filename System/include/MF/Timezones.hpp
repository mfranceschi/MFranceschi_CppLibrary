//
// Created by MartinF on 12/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_TIMEZONES_HPP
#define MFRANCESCHI_CPPLIBRARIES_TIMEZONES_HPP

#include "DateTypes.hpp"

namespace MF
{
    namespace Timezones
    {
        /**
         * Return the offset of the current timezone in regards to the UTC time zone.
         * Generally a number of hours, sometimes has a minutes part as well.
         * As defined in https://pubs.opengroup.org/onlinepubs/7908799/xsh/timezone.html.
         */
        DateTypes::TimezoneOffset_t getTimezoneOffset();

        /**
         * Returns the length of the DST offset for the current timezone.
         * Usually 1 hour. 0 if DST does not apply in the current timezone.
         */
        DateTypes::Interval_t getDstOffset();
    } // namespace Timezones
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_TIMEZONES_HPP
