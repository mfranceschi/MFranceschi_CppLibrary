//---------- Interface of class <Date> (file Date.h) ----------------
#if ! defined ( DATE_H )
#define DATE_H

//--------------------------------------------------------------- Includes
#include <ctime> /* for tm and time_t */
#include <ostream> /* for operator << */
#include <string> /* for conversions with strings */
#include <type_traits> /* for type checking in template MakeMS */
#include "Toolbox.hpp"

// YOU CAN CHANGE THIS VALUE HERE.
// THIS IS A FLAG FOR ENABLING MICROSECONDS MANAGEMENT.
// Accepted values: 0 for disabling, 1 for enabling.
#define DATE_MIC_ON 0

#if !(DATE_MIC_ON == 1 || DATE_MIC_ON == 0)
#error "The macro DATE_MIC_ON must be defined and have the value 0 or 1."
#endif

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

#if DATE_MIC_ON == 1
	typedef unsigned int MicroSeconds; // Type for representing microseconds.
	typedef double Interval; // Time distance between two dates, in seconds.
	#define _MicroSeconds
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

	// Input checking utilities.
	/*static constexpr bool ValidateSeconds(int) noexcept;
	static constexpr bool ValidateMinutes(int) noexcept;
	static constexpr bool ValidateHours(int) noexcept;
	static constexpr bool ValidateDays(int days, int month = -1) noexcept;
	static constexpr bool ValidateMonths(int) noexcept;
	*/

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

	constexpr static int NBR_SECONDS_IN_DAY = 24 * 60 * 60; // Number of seconds in a single day.

	constexpr static int DST_UNKNOWN = -1, DST_OFF = 0, DST_ON = 1; // Values for the DST flag.

	const static int MAX_YEAR; // Max possible year on this computer.



#ifdef _MicroSeconds
	constexpr static MicroSeconds MS_MAX = 1000000; // No more than 1M microseconds to be stored!
	constexpr static char NO_MS = 0x00; // Use this in ms_sep if you don't want to represent microseconds in string.

	template <typename T> constexpr static MicroSeconds MakeMS(T val); // Throws WRONG_MS if not in right range.
	static MicroSeconds ms_tolerance(MicroSeconds = -1); // Refers to Date::tolerance.
	static char ms_CharSep(char = '\r'); // Sets the msCharSep.
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

	int seconds(int newvalue = -1); // 0-60
	int minutes(int newvalue = -1); // 0-59
	int hours(int newvalue = -1); // 0-23
	int day_month(int newvalue = -1); // 1-31
	int month(int newvalue = -1); // 0-11

	// Day of week (since Sunday), 0-6. Read-only.
	inline int day_week() const;

	// Day since January 1st, 0-365. Read-only.
	inline int day_year() const; 

	// Years since 1900.
	int year(int); inline int year() const;

	// Daylight Saving Time flag. Values: -1, 0, 1.
	int dst(int); inline int dst() const;

	static const char* str_pattern(const char* pattern = ""); // Refers to Date::pattern. Empty string = getter.

#ifdef _MicroSeconds
	static char Date::ms_CharSep(char newsep);
	MicroSeconds microseconds(MicroSeconds = -1);
#endif


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

#ifdef _MicroSeconds
	explicit Date(tm, MicroSeconds = 0);
	explicit Date(time_t, MicroSeconds = 0);
	explicit Date(const std::string&, const char* pattern, MicroSeconds = 0);
	explicit Date(int year, int month, int monthday, int hour, int minutes, int seconds, int dst_flag, MicroSeconds ms = 0);
#else
	explicit Date(tm);
	explicit Date(time_t);
	explicit Date(const std::string&, const char* pattern);
	explicit Date(int year, int month, int monthday, int hour, int minutes, int seconds, int dst_flag);
#endif

	explicit Date(); // Holds time from app launch.
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
	
#ifdef _MicroSeconds
	MicroSeconds microseconds_in; // Instance field that holds microseconds.
	static MicroSeconds tolerance; // Tolerance in microseconds. Initial value of MS_MAX.
	static char msSepChar; // Separator for normal datetime and mics in strings. Initial value of NO_MS.
#endif
};

//------------------------------------------------------ Other definitions

#ifdef _MicroSeconds
template <typename T>
constexpr MicroSeconds Date::MakeMS(T val)
{
	static_assert(std::is_arithmetic<T>::value, "Argument must be an arithmeric value.");
	MicroSeconds valMS = static_cast<MicroSeconds>(val);
	return valMS < MS_MAX ? valMS : throw DateError::WRONG_MS;
}
#endif

constexpr bool Date::IsLeapYear(int year) noexcept
{	return (!(year & 0b11) && (year % 100 != 0)) || (year % 400 == 0); }

/*
constexpr bool Date::ValidateSeconds(int d) noexcept
{	return d >= 0 && d <= 60; }

constexpr bool Date::ValidateMinutes(int d) noexcept
{	return d >= 0 && d < 60; }

constexpr bool Date::ValidateHours(int d) noexcept
{	return d >= 0 && d < 24; }

constexpr bool Date::ValidateDays(int d, int m) noexcept
{	return d >= 1 && d <= (Date::ValidateMonths(m) ? d <= Date::DaysInMonth(m) : 31); }

constexpr bool Date::ValidateMonths(int d) noexcept
{	return d >= 0 && d < 12; }
*/

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
{
	return os << std::string(d);
}

#ifdef _MicroSeconds
#undef _MicroSeconds
#endif

#endif // DATE_H
