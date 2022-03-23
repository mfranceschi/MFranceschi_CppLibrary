//
// Created by MartinF on 06/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATEUTILS_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATEUTILS_HPP

#include <ctime>

namespace MF
{
    namespace DateUtils
    {
        /** Constants for months. */
        constexpr static int JANUARY = 0, FEBRUARY = 1, MARCH = 2, APRIL = 3, MAY = 4, JUNE = 5,
                             JULY = 6, AUGUST = 7, SEPTEMBER = 8, OCTOBER = 9, NOVEMBER = 10,
                             DECEMBER = 11;

        /** Constants for days of week. */
        constexpr static int SUNDAY = 0, MONDAY = 1, TUESDAY = 2, WEDNESDAY = 3, THURSDAY = 4,
                             FRIDAY = 5, SATURDAY = 6;

        /** Constants for the DST flag. */
        constexpr static int DST_UNKNOWN = -1, DST_OFF = 0, DST_ON = 1;

        /** Number of seconds in a single day. */
        constexpr static int NBR_SECONDS_IN_DAY = 24 * 60 * 60;

        /** Max possible year on this computer. */
        extern const int MAX_YEAR;

        /** Max possible value for microseconds (EXCLUSIVE). */
        constexpr static int MS_MAX = 1000000;

        /** Use this in ms_sep if you don't want to represent microseconds in string. */
        constexpr static char NO_MS = '\x00';

        /** Returns true if the given year is a leap year. */
        constexpr bool IsLeapYear(int year) noexcept {
            return (!(year & 0b11) && (year % 100)) || !(year % 400);
        }

        /** Returns [28 - 31] or 0 if month is not valid. */
        constexpr int DaysInMonth(int month, int year = 0) noexcept {
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

        /** Returns whether the input is a valid value for microseconds. */
        constexpr bool ValidateMicroseconds(int value) noexcept {
            return value >= 0 && value < MS_MAX;
        }

        /** Returns whether the input is a valid value for seconds. */
        constexpr bool ValidateSeconds(int value) noexcept {
            return value >= 0 && value <= 60;
        }

        /** Returns whether the input is a valid value for minutes. */
        constexpr bool ValidateMinutes(int value) noexcept {
            return value >= 0 && value < 60;
        }

        /** Returns whether the input is a valid value for hours. */
        constexpr bool ValidateHours(int value) noexcept {
            return value >= 0 && value <= 24;
        }

        /** Returns whether the input is a valid value for month. */
        constexpr bool ValidateMonths(int value) noexcept {
            return value >= JANUARY && value <= DECEMBER;
        }

        /** Returns whether the input is a valid value for days of month. */
        constexpr bool ValidateDays(int days, int month) noexcept {
            auto daysInMonth = ValidateMonths(month) ? days <= DaysInMonth(month) : 31;
            return days >= 1 && days <= daysInMonth;
        }

        /** Returns now as UTC timestamp. */
        inline time_t Now_Timestamp() {
            return time(nullptr);
        }

        /** Thread-safe version of Localtime. */
        bool Localtime(const std::time_t &src, struct std::tm &dest);

        /** Thread-safe version of Gmtime. */
        bool Gmtime(const std::time_t &src, struct std::tm &dest);

        /** Returns the difference in seconds between UTC and local time. */
        long Timezone();
    } // namespace DateUtils
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_DATEUTILS_HPP
