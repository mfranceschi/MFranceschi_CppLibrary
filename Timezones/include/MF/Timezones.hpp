//
// Created by MartinF on 18/06/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_TIMEZONES_HPP
#define MFRANCESCHI_CPPLIBRARIES_TIMEZONES_HPP

#include <chrono>
#include <string>

namespace MF
{
    namespace Timezones
    {
        /**
         * Return the offset of the current timezone in regards to the UTC time zone.
         * As defined in https://pubs.opengroup.org/onlinepubs/7908799/xsh/timezone.html.
         *
         * Example: UTC+1 --> 1 hour.
         */
        std::chrono::seconds getTimezoneOffset();

        /**
         * Returns the length of the DST offset for the current timezone.
         * If 0 then DST does not apply in the current timezone.
         * Generally equal to 1 hour.
         */
        std::chrono::seconds getDstOffset();

        /**
         * Returns the name of the current timezone.
         * It should be a timezone name independent from the DST data.
         *
         * Example: "Europe/Paris".
         */
        std::string getTimezoneName();

        /**
         * Sets the new value for the TZ environment variable.
         * Format is system dependent.
         * If "runTzSet" is true then calls "tzset".
         *
         * Unix - https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
         * Windows - https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/tzset#remarks
         */
        void setSystemTz(const std::string& newValue, bool runTzSet = true);

        /**
         * Returns the current value of the TZ environment variable.
         * Throws a system_error if it does not exist.
         *
         * Format is system dependent.
         * Unix - https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
         * Windows - https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/tzset#remarks
         */
        std::string getSystemTz();

        /** See "tzset". */
        void tzSet();
    } // namespace Timezones
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_TIMEZONES_HPP
