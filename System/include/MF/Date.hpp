//---------- Interface of class <Date> (file Date.h) ----------------
#if !defined ( DATE_H )
#define DATE_H

//--------------------------------------------------------------- Includes
#include <ctime> /* for tm and time_t */
#include <ostream> /* for operator << */
#include <string> /* for conversions with strings */
#include <type_traits> /* for type checking in template MakeMS */

#include "MF/DateTypes.hpp"
#include "MF/DateUtils.hpp"

namespace MF {

//------------------------------------------------------------------ Types

    typedef double Interval; // Time distance between two dates, in seconds.

    // Role of Date:
    // Simple extension of the C struct tm with microseconds and operators overloading.
    class Date {
        //----------------------------------------------------------------- PUBLIC

    public:
        //------------------------------------------------------- Static utilities

        static int ms_tolerance() noexcept;

        static int ms_tolerance(int);

        static char ms_CharSep() noexcept;

        static char ms_CharSep(char);

        //--------------------------------------------------------- Public methods

        // Compares the current date with the given one.
        // Returns Dates::INFERIOR if "this" is before "param",
        // same idea for Dates::EQUAL and Dates::SUPERIOR.
        // The "tolerance" variable is used here.
        int Compare(const Date &) const;

        // Returns (this) - (param) in seconds.
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

        static const char *str_pattern(const char *pattern = ""); // Refers to Date::pattern. Empty string = getter.

        //-------------------------------------------------- Operator overloadings

        // Comparison operators.
        bool operator==(const Date &b) const;

        inline bool operator!=(const Date &b) const;

        inline bool operator<(const Date &b) const;

        inline bool operator>(const Date &b) const;

        inline bool operator<=(const Date &b) const;

        inline bool operator>=(const Date &b) const;

        // Returns interval.
        inline Interval operator%(const Date &b) const;

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

        explicit Date(time_t, int microseconds = 0);

        explicit Date(const std::string &, const char *pattern, int microseconds = 0);

        explicit Date(int year, int month, int monthday, int hour, int minutes, int seconds, int dst_flag,
                      int microseconds = 0);

        explicit Date(const std::string &, const char *pattern);

        /** Returns the current time as a date. */
        static Date Now() noexcept;

        static inline Date FromTime_t(time_t time1, int microseconds = 0);

        explicit Date();

        virtual ~Date() = default;

//---------------------------------------------------------------- PRIVATE

    protected:
//------------------------------------------------------ Protected methods

        // If newvalue < max: sets field then returns it.
        // If newvalue == -1: returns field.
        // Else throw DateError_e::WRONG_TIME_DATA.
        int quickSetter(int newvalue, int min, int max, int &field);
//--------------------------------------------------- Protected attributes

        tm time{}; // tm struct that holds the current date.
        time_t timet; // time_t that holds the current date.
        static const char *pattern; // String pattern based on strftime. No microseconds here.
        static Date lastCallToNow; // All default-constructed Date instances will have this value. Updated on each call to Now.

        int microseconds_in; // Instance field that holds microseconds.
        static int tolerance; // Tolerance in microseconds. Initial value of MS_MAX.
        static char msSepChar; // Separator for normal datetime and mics in strings. Initial value of NO_MS.
    };

//------------------------------------------------------ Other definitions

    inline int Date::seconds() const { return time.tm_sec; }

    inline int Date::minutes() const { return time.tm_min; }

    inline int Date::hours() const { return time.tm_hour; }

    inline int Date::day_month() const { return time.tm_mday; }

    inline int Date::month() const { return time.tm_mon; }

    inline int Date::day_week() const { return time.tm_wday; }

    inline int Date::day_year() const { return time.tm_yday; }

    inline int Date::year() const { return time.tm_year; }

    inline int Date::dst() const { return time.tm_isdst; }

    inline bool Date::operator!=(const Date &b) const { return !(*this == b); }

    inline bool Date::operator<(const Date &b) const { return Compare(b) == -1; }

    inline bool Date::operator>(const Date &b) const { return Compare(b) == +1; }

    inline bool Date::operator<=(const Date &b) const { return Compare(b) != +1; }

    inline bool Date::operator>=(const Date &b) const { return Compare(b) != -1; }

    inline Interval Date::operator%(const Date &b) const { return Timedelta(b); }

    inline Date &Date::operator++() {
        seconds(seconds() + 1);
        return *this;
    }

    inline Date Date::operator++(int) {
        Date tmp(*this);
        operator++();
        return tmp;
    }

    inline Date::operator struct tm() const { return time; }

    inline Date::operator time_t() const { return timet; }

    inline Date Date::FromTime_t(const time_t time1, int microseconds) {
        std::tm tm1{0};
        DateUtils::Gmtime(time1, tm1);
        return Date(tm1);
    }

    inline std::ostream &operator<<(std::ostream &os, const Date &d) { return os << std::string(d); }

}
#endif // DATE_H
