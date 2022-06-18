//
// Created by Utilisateur on 25/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATETIME_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATETIME_HPP

#include <ctime>
#include <iostream>
#include <stdexcept>

namespace MF
{
    namespace Datetime
    {
        /**
         * Immutable class representing a distance between two time points.
         */
        struct Interval {
            const std::time_t seconds;
            const std::uint32_t nanos;

            constexpr explicit Interval(std::time_t seconds = 0, std::uint32_t nanos = 0)
                : seconds(seconds), nanos(nanos) {
                // Note: we don't check for the value of 'nanos' to be < 1e9 to limit overhead.
                // TODO check lol
            }

            constexpr explicit operator double() const {
                return double(seconds) + 1e9 / double(nanos);
            }

            constexpr explicit operator std::uint32_t() const {
                return seconds;
            }

            constexpr unsigned int getMicrosecondsPart() const {
                return nanos / (unsigned int)1e3;
            }

            constexpr unsigned int getMillisecondsPart() const {
                return nanos / (unsigned int)1e6;
            }

            constexpr unsigned int getSecondsPart() const {
                return seconds % 60;
            }

            constexpr unsigned int getMinutesPart() const {
                return (seconds / 60) % 60;
            }

            constexpr unsigned int getHoursPart() const {
                return (seconds / 3600) % 3600;
            }

            constexpr unsigned int getDaysPart() const {
                return seconds / 86400;
            }

            constexpr Interval operator+(const Interval &other) const {
                std::uint32_t newNanos = this->nanos + other.nanos;
                std::time_t newSeconds = this->seconds + other.seconds;
                if (newNanos > std::uint32_t(1e9)) {
                    newNanos -= std::uint32_t(1e9);
                    newSeconds++;
                }
                return Interval(newSeconds, newNanos);
            }

            constexpr bool operator==(const Interval &other) const {
                return this->nanos == other.nanos && this->seconds == other.seconds;
            }
        };

        using Interval_t = Interval;
        using TimezoneOffset_t = long;

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
            DateError(DateError_e enumValue, const std::string &errorText)
                : std::runtime_error(errorText), errorValue(enumValue) {
            }

            const DateError_e &getErrorValue() const {
                return errorValue;
            }
        };

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
            return std::time(nullptr);
        }

        /** Thread-safe version of Localtime. */
        bool Localtime(const std::time_t &src, struct std::tm &dest);

        /** Thread-safe version of Gmtime. */
        bool Gmtime(const std::time_t &src, struct std::tm &dest);

        /** Returns the difference in seconds between UTC and local time. */
        long Timezone();

        void SetTZ(const std::string &tzValue);
        std::string GetTZ();

        /** Simple extension of the C struct tm with microseconds and operators overloading. */
        class Date {
            //----------------------------------------------------------------- PUBLIC

           public:
            //------------------------------------------------------- Static utilities

            static int ms_tolerance() noexcept;

            static int ms_tolerance(int);

            static char ms_CharSep() noexcept;

            static char ms_CharSep(char);

            //--------------------------------------------------------- Public methods

            /**
             * Compares the current date with the given one.
             * Returns Dates::INFERIOR if "this" is before "param",
             * same idea for Dates::EQUAL and Dates::SUPERIOR.
             * The "tolerance" variable is used here.
             */
            int Compare(const Date &) const;

            /** Returns (this) - (param) in seconds. */
            Interval Timedelta(const Date &) const;

            // Getters and setters.
            // To get, call without parameter.
            // To set, call with a positive integer.
            // Return value: always the final value.
            // Throws an DateError_e::WRONG_TIME_DATA if data is invalid.

            inline int microseconds() const;

            int microseconds(int);

            inline int seconds() const;

            int seconds(int);

            inline int minutes() const;

            int minutes(int);

            inline int hours() const;

            int hours(int);

            inline int day_month() const;

            int day_month(int);

            inline int month() const;

            int month(int);

            // Day of week (since Sunday), 0-6. Read-only.
            inline int day_week() const;

            // Day since January 1st, 0-365. Read-only.
            inline int day_year() const;

            // Years since 1900.
            inline int year() const;

            int year(int);

            // Daylight Saving Time flag. Values: -1, 0, 1.
            inline int dst() const;

            int dst(int);

            /** Refers to Date::pattern. Empty string = getter. */
            static const char *str_pattern(const char *pattern = "");

            //-------------------------------------------------- Operator overloadings

            // Comparison operators.
            bool operator==(const Date &other) const;

            inline bool operator!=(const Date &other) const;

            inline bool operator<(const Date &other) const;

            inline bool operator>(const Date &other) const;

            inline bool operator<=(const Date &other) const;

            inline bool operator>=(const Date &other) const;

            // Returns interval.
            inline Interval operator%(const Date &other) const;

            inline Interval operator-(const Date &b) const;

            // Arithmetics
            Date operator+(Interval seconds) const;

            Date operator-(Interval seconds) const;

            Date &operator+=(Interval seconds);

            Date &operator-=(Interval seconds);

            inline Date &operator++(); // +1 second
            inline Date operator++(int); // +1 second

            // Conversions and stream
            inline explicit operator struct tm() const;

            inline explicit operator time_t() const;

            explicit operator std::string() const;

            inline friend std::ostream &operator<<(std::ostream &, const Date &);

            //---------------------------------------------- Constructors - destructor

            Date(const Date &) = default;

            explicit Date(tm, int microseconds = 0);

            explicit Date(const std::string &, const char *pattern, int microseconds = 0);

            explicit Date(
                int year,
                int month,
                int monthday,
                int hour,
                int minutes,
                int seconds,
                int dst_flag,
                int microseconds = 0);

            explicit Date(const std::string &, const char *pattern);

            /** Returns the current time as a date. */
            static Date Now() noexcept;

            static inline Date FromTime_t(time_t time1, int microseconds = 0);

            explicit Date();

            virtual ~Date() = default;

            //---------------------------------------------------------------- PRIVATE

           private:
            //------------------------------------------------------ Protected methods

            /** If newValue < max: sets field then returns it.
             * If newValue == -1: returns field.
             * Else throw DateError_e::WRONG_TIME_DATA. */
            int quickSetter(int newValue, int min, int max, int &field);
            //--------------------------------------------------- Protected attributes

            /** tm struct that holds the current date. */
            tm time{};

            /** time_t that holds the current date. */
            time_t timet;

            /** String pattern based on strftime. No microseconds here. */
            static const char *pattern;

            /** All default-constructed Date instances will have this value.
             * Updated on each call to Now. */
            static Date lastCallToNow;

            /** Instance field that holds microseconds. */
            int microseconds_in{};

            /** Tolerance in microseconds.
             * Initial value of MS_MAX. */
            static int tolerance;

            /** Separator for normal datetime and mics in strings.
             * Initial value of NO_MS. */
            static char msSepChar;
        };

        //------------------------------------------------------ Other definitions

        inline int Date::seconds() const {
            return time.tm_sec;
        }

        inline int Date::minutes() const {
            return time.tm_min;
        }

        inline int Date::hours() const {
            return time.tm_hour;
        }

        inline int Date::day_month() const {
            return time.tm_mday;
        }

        inline int Date::month() const {
            return time.tm_mon;
        }

        inline int Date::day_week() const {
            return time.tm_wday;
        }

        inline int Date::day_year() const {
            return time.tm_yday;
        }

        inline int Date::year() const {
            return time.tm_year;
        }

        inline int Date::dst() const {
            return time.tm_isdst;
        }

        inline bool Date::operator!=(const Date &other) const {
            return !(*this == other);
        }

        inline bool Date::operator<(const Date &other) const {
            return Compare(other) == -1;
        }

        inline bool Date::operator>(const Date &other) const {
            return Compare(other) == +1;
        }

        inline bool Date::operator<=(const Date &other) const {
            return Compare(other) != +1;
        }

        inline bool Date::operator>=(const Date &other) const {
            return Compare(other) != -1;
        }

        inline Interval Date::operator%(const Date &other) const {
            return Timedelta(other);
        }

        inline Date &Date::operator++() {
            seconds(seconds() + 1);
            return *this;
        }

        inline Date Date::operator++(int) {
            Date tmp(*this);
            operator++();
            return tmp;
        }

        inline Date::operator struct tm() const {
            return time;
        }

        inline Date::operator time_t() const {
            return timet;
        }

        inline Date Date::FromTime_t(const time_t time1, int microseconds) {
            std::tm tm1{0};
            Localtime(time1, tm1);
            return Date(tm1, microseconds);
        }

        inline std::ostream &operator<<(std::ostream &ostream, const Date &date) {
            return ostream << std::string(date);
        }
    } // namespace Datetime
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_DATETIME_HPP
