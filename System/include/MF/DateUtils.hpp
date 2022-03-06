//
// Created by MartinF on 06/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATEUTILS_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATEUTILS_HPP

namespace MF {
    namespace DateUtils {
        namespace internal {
            template<typename type = long long>
            constexpr int Sign(type nbr) {
                return !nbr ? 0 :
                       nbr < static_cast<type>(0) ?
                       -1 : +1;
            }
        }

        // Months (to be used with the Month getter and/or setter.)
        constexpr static int
                JANUARY = 0, FEBRUARY = 1,
                MARCH = 2, APRIL = 3,
                MAY = 4, JUNE = 5,
                JULY = 6, AUGUST = 7,
                SEPTEMBER = 8, OCTOBER = 9,
                NOVEMBER = 10, DECEMBER = 11;

        // Days of week.
        constexpr static int
                SUNDAY = 0, MONDAY = 1,
                TUESDAY = 2, WEDNESDAY = 3,
                THURSDAY = 4, FRIDAY = 5,
                SATURDAY = 6;

        constexpr static int DST_UNKNOWN = -1, DST_OFF = 0, DST_ON = 1; // Values for the DST flag.

        constexpr static int NBR_SECONDS_IN_DAY = 24 * 60 * 60; // Number of seconds in a single day.

        const static int MAX_YEAR = 0; // Max possible year on this computer.

        constexpr static int MS_MAX = 1000000; // No more than 1M microseconds to be stored!
        constexpr static char NO_MS = '\x00'; // Use this in ms_sep if you don't want to represent microseconds in string.

        /** Returns true if the given year is a leap year. */
        static constexpr bool IsLeapYear(int year) noexcept {
            return (!(year & 0b11) && (year % 100)) || !(year % 400);
        }

        /** Returns [28 - 31] or 0 if month is not valid. */
        static constexpr int DaysInMonth(int month, int year = 0) noexcept {
            switch (month) {
                case JANUARY:
                case MARCH:
                case MAY:
                case JULY:
                case AUGUST:
                case OCTOBER:
                case DECEMBER:
                    return 31;
                case APRIL:
                case JUNE:
                case SEPTEMBER:
                case NOVEMBER:
                    return 30;
                case FEBRUARY:
                    return IsLeapYear(year) ? 29 : 28;
                default:
                    return 0;
            }
        }

        // Input checking utilities.
        static constexpr bool ValidateMicroseconds(int value) noexcept { return value >= 0 && value < MS_MAX; }

        static constexpr bool ValidateSeconds(int value) noexcept { return value >= 0 && value <= 60; }

        static constexpr bool ValidateMinutes(int value) noexcept { return value >= 0 && value <= 60; }

        static constexpr bool ValidateHours(int value) noexcept { return value >= 0 && value <= 24; }

        static constexpr bool ValidateMonths(int value) noexcept { return value >= JANUARY && value <= DECEMBER; }

        static constexpr bool ValidateDays(int days, int month) noexcept {
            auto daysInMonth = ValidateMonths(month) ? days <= DaysInMonth(month) : 31;
            return days >= 1 && days <= daysInMonth;
        }

    }
}

#endif //MFRANCESCHI_CPPLIBRARIES_DATEUTILS_HPP
