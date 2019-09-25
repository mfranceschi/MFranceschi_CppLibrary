//---------- Interface of class <Date> (file Date.h) ----------------
#if ! defined ( DATE_H )
#define DATE_H

//--------------------------------------------------------------- Includes
#include <ctime> /* for tm and time_t */
#include <ostream> /* for operator << */
#include <string> /* for conversions with strings */
#include <type_traits> /* for type checking in template MakeMS */
#include "Toolbox.hpp"

// THIS IS A FLAG FOR ENABLING MICROSECONDS MANAGEMENT.
// Remove the following flag (e.g. by commenting or deleting the line) to disable.
#define DATE_MIC_ON

//------------------------------------------------------------------ Types

// Errors for this module. Don't use the integer values directly.
enum DateError
{
	NO_PATTERN,			// No pattern was available to perform the Date<->string conversion.
	WRONG_STRUCT_TM,	// Struct tm was corrupted.
	WRONG_MS,			// The number of milliseconds was not acceptable (too high?).
	WRONG_STRING,		// String could not be retrieved as Date using the given pattern.
	WRONG_TIME_T,		// The given time_t could not be converted as date (value too high?).
	WRONG_TIME_DATA		// Returned by setter functions if the parameter is not in the correct range.
};

#ifdef DATE_MIC_ON 
	typedef double Interval; // Time distance between two dates, in seconds.
#else
	typedef time_t Interval; // Time distance between two dates, in seconds.
#endif


// Role of Date: 
// Simple extension of the C struct tm with microseconds and operators overloading. 
class Date
{
	//----------------------------------------------------------------- PUBLIC

public:
	//------------------------------------------------------- Static utilities
	
	static constexpr bool IsLeapYear(int year) noexcept; // Returns true if the given year is a leap year.
	static int DaysInMonth(int month, int year = 0) noexcept; // Returns [28 - 31] or 0 if month is not valid.
	static Date Now(); // Returns the current time as a date (makes a syscall).
	static inline time_t Now_Timestamp(); // Returns now as UTC timestamp.
	static bool Localtime(const time_t& src, struct tm& dest); // Thread-safe version of Localtime.
	static bool Gmtime(const time_t& src, struct tm& dest); // Thread-safe version of Gmtime.
	static long Timezone(); // Returns the difference in seconds between UTC and local time.

	// Input checking utilities.
	static constexpr bool ValidateSeconds(int) noexcept;
	static constexpr bool ValidateMinutes(int) noexcept;
	static constexpr bool ValidateHours(int) noexcept;
	static constexpr bool ValidateDays(int days, int month) noexcept;
	static constexpr bool ValidateMonths(int) noexcept;
#ifdef DATE_MIC_ON
	static constexpr bool ValidateMicroseconds(int) noexcept;
#endif

	// Months (to be used with the Month getter and/or setter.)
	constexpr static int 
		JANUARY = 0,    FEBRUARY = 1,
		MARCH = 2,		APRIL = 3,
		MAY = 4,		JUNE = 5,
		JULY = 6,		AUGUST = 7,
		SEPTEMBER = 8,	OCTOBER = 9,
		NOVEMBER = 10,	DECEMBER = 11;

	// Days of week.
	constexpr static int
		SUNDAY = 0,		MONDAY = 1, 
		TUESDAY = 2,	WEDNESDAY = 3, 
		THURSDAY = 4,	FRIDAY = 5, 
		SATURDAY = 6;

	constexpr static int DST_UNKNOWN = -1, DST_OFF = 0, DST_ON = 1; // Values for the DST flag.
	
	constexpr static int NBR_SECONDS_IN_DAY = 24 * 60 * 60; // Number of seconds in a single day.

	const static int MAX_YEAR; // Max possible year on this computer.

#ifdef DATE_MIC_ON
	constexpr static int MS_MAX = 1000000; // No more than 1M microseconds to be stored!
	constexpr static char NO_MS = '0x00'; // Use this in ms_sep if you don't want to represent microseconds in string.

	static int ms_tolerance() noexcept;
	static int ms_tolerance(int); 
	static char ms_CharSep() noexcept;
	static char ms_CharSep(char); 
#endif

	//--------------------------------------------------------- Public methods

	// Compares the current date with the given one.
	// Returns Dates::INFERIOR if "this" is before "param",
	// same idea for Dates::EQUAL and Dates::SUPERIOR.
	// The "tolerance" variable is used here.
	int Compare(const Date&) const;

	// Returns (this) - (param) in seconds.
	Interval Timedelta(const Date&) const;

	// Getters and setters.
	// To get, call without parameter.
	// To set, call with a positive integer.
	// Return value: always the final value.
	// Throws an DateError::WRONG_TIME_DATA if data is invalid.

#ifdef DATE_MIC_ON
	inline int microseconds() const; int microseconds(int);
#endif
	inline int seconds() const; int seconds(int);
	inline int minutes() const; int minutes(int);
	inline int hours() const; int hours(int);
	inline int day_month() const; int day_month(int);
	inline int month() const; int month(int);

	// Day of week (since Sunday), 0-6. Read-only.
	inline int day_week() const;

	// Day since January 1st, 0-365. Read-only.
	inline int day_year() const; 

	// Years since 1900.
	inline int year() const; int year(int);

	// Daylight Saving Time flag. Values: -1, 0, 1.
	inline int dst() const; int dst(int);

	static const char* str_pattern(const char* pattern = ""); // Refers to Date::pattern. Empty string = getter.

	//-------------------------------------------------- Operator overloadings
	
	// Comparison operators.
	bool operator== (const Date& b) const;
	inline bool operator!= (const Date& b) const;
	inline bool operator<  (const Date& b) const;
	inline bool operator>  (const Date& b) const;
	inline bool operator<= (const Date& b) const;
	inline bool operator>= (const Date& b) const;

	// Returns interval.
	inline Interval operator% (const Date& b) const;
	inline Interval operator- (const Date& b) const;

	// Arithmetics
	Date operator+ (Interval seconds) const;
	Date operator- (Interval seconds) const;
	Date& operator+= (Interval seconds);
	Date& operator-= (Interval seconds);
	inline Date& operator++(); // +1 second
	inline Date operator++(int); // +1 second

	// Conversions and stream
	inline operator struct tm() const;
	inline operator time_t () const;
	operator std::string() const;
	inline friend std::ostream& operator<<(std::ostream&, const Date&);

//---------------------------------------------- Constructors - destructor
    
    Date(const Date&) = default;

#ifdef DATE_MIC_ON
	explicit Date(tm, int microseconds = 0);
	explicit Date(time_t, int microseconds = 0);
	explicit Date(const std::string&, const char* pattern, int microseconds = 0);
	explicit Date(int year, int month, int monthday, int hour, int minutes, int seconds, int dst_flag, int microseconds = 0);
#else
	explicit Date(tm);
	explicit Date(time_t);
	explicit Date(const std::string&, const char* pattern);
	explicit Date(int year, int month, int monthday, int hour, int minutes, int seconds, int dst_flag);
#endif

	explicit Date();
	virtual ~Date() = default;

//---------------------------------------------------------------- PRIVATE

protected:
//------------------------------------------------------ Protected methods
	
	// If newvalue < max: sets field then returns it.
	// If newvalue == -1: returns field.
	// Else throw DateError::WRONG_TIME_DATA.
	int quickSetter(int newvalue, int min, int max, int& field);
//--------------------------------------------------- Protected attributes

	tm time; // tm struct that holds the current date.
	time_t timet; // time_t that holds the current date.
	static const char* pattern; // String pattern based on strftime. No microseconds here.
	static Date lastCallToNow; // All default-constructed Date instances will have this value. Updated on each call to Now.
	
#ifdef DATE_MIC_ON
	int microseconds_in; // Instance field that holds microseconds.
	static int tolerance; // Tolerance in microseconds. Initial value of MS_MAX.
	static char msSepChar; // Separator for normal datetime and mics in strings. Initial value of NO_MS.
#endif
};

//------------------------------------------------------ Other definitions

constexpr bool Date::IsLeapYear(int year) noexcept
{	return (!(year & 0b11) && (year % 100 != 0)) || (year % 400 == 0); }

#ifdef DATE_MIC_ON
constexpr bool Date::ValidateMicroseconds(int d) noexcept
{	return d >= 0 && d < MS_MAX; }
#endif

constexpr bool Date::ValidateSeconds(int d) noexcept
{	return d >= 0 && d <= 60; }

constexpr bool Date::ValidateMinutes(int d) noexcept
{	return d >= 0 && d < 60; }

constexpr bool Date::ValidateHours(int d) noexcept
{	return d >= 0 && d < 24; }

constexpr bool Date::ValidateDays(int d, int m) noexcept
{	return d >= 1 && d <= (Date::ValidateMonths(m) ? d <= Date::DaysInMonth(m) : 31); }

constexpr bool Date::ValidateMonths(int d) noexcept
{	return d >= JANUARY && d <= DECEMBER; }

inline time_t Date::Now_Timestamp()
{ 	return std::time(nullptr); }

inline int Date::day_week() const 
{	return time.tm_wday; }

inline int Date::day_year() const
{	return time.tm_yday; }

inline int Date::year() const 
{	return time.tm_year; }

inline int Date::dst() const 
{	return time.tm_isdst; }

inline bool Date::operator!= (const Date& b) const 
{	return !(*this == b); }

inline bool Date::operator< (const Date& b) const 
{	return Compare(b) == Toolbox::INFERIOR; }

inline bool Date::operator>  (const Date& b) const 
{	return Compare(b) == Toolbox::SUPERIOR; }

inline bool Date::operator<= (const Date& b) const 
{	return Compare(b) != Toolbox::SUPERIOR; }

inline bool Date::operator>= (const Date& b) const 
{	return Compare(b) != Toolbox::INFERIOR; }

inline Interval Date::operator% (const Date& b) const 
{	return Timedelta(b); }

inline Date& Date::operator++() 
{	
	seconds(seconds() + 1); 
	return *this; 
} 

inline Date Date::operator++(int) 
{	
	Date tmp(*this); 
	operator++(); 
	return tmp; 
}

inline Date::operator struct tm() const 
{	return time; }

inline Date::operator time_t () const 
{	return timet; }

inline std::ostream& operator<<(std::ostream& os, const Date& d)
{	return os << std::string(d); }

#endif // DATE_H
