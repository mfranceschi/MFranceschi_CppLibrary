//---------- Interface of class <Date> (file Date.h) ----------------
#if ! defined ( DATE_H )
#define DATE_H

//--------------------------------------------------------------- Includes
#include <ctime> /* for tm and time_t */
#include <string> /* for conversions with strings */
#include "Toolbox.h"

// Type for representing years

//------------------------------------------------------- Public constants

// YOU CAN CHANGE THIS VALUE HERE.
// THIS IS A FLAG FOR ENABLING MICROSECONDS MANAGEMENT.
// Accepted values: 0 for disabling, 1 for enabling.
#define DATE_MIC_ON 1

#if !(DATE_MIC_ON == 1 || DATE_MIC_ON == 0)
#error "The macro DATE_MIC_ON must be defined and have the value 0 or 1."
#endif

//------------------------------------------------------------------ Types

// Error codes for this module. Don't use integer values directly.
enum DateError
{
	NO_PATTERN = 1, // No pattern was available to perform the Date<->string conversion.
	WRONG_STRUCT_TM = 2, // Struct tm was corrupted.
	WRONG_MS = 3, // The number of milliseconds was not acceptable (too high?).
	WRONG_STRING = 4, // String could not be retrieved as Date using the given pattern.
	WRONG_TIME_T = 5, // The given time_t could not be converted as date (value too high?).
	WRONG_TIME_DATA = 6 // Returned by setter functions if the parameter is not in the correct range.
};

#if DATE_MIC_ON == 1
typedef unsigned int MicroSeconds; // Type for representing microseconds.
typedef long double Interval; // Time distance between two dates, in seconds.
#else
typedef time_t Interval; // Time distance between two dates, in seconds.
#endif


// Role of Date: 
// Simple extension of C's struct tm with microseconds and operator 
// overloadings. 
class Date
{
//----------------------------------------------------------------- PUBLIC

public:
//--------------------------------------------------------- Public methods

	constexpr static bool IsLeapYear(int year) 
	{ return (!(year & 0b11) && (year % 100 != 0)) || (year % 400 == 0); }

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
	// With declarations, you can find the accepted bounds of values.
	
	int seconds(int newvalue = -1); // 0-59
	int minutes(int newvalue = -1); // 0-59
	int hours(int newvalue = -1); // 0-23
	int monthday(int newvalue = -1); // 1-31
	int month(int newvalue = -1); // 0-11
	int weekday(int newvalue = -1); // 0-6, days since Sunday
	int yearday(int newvalue = -1); // 0-365
	int year(int newvalue = INT_MIN); // Years since 1900,
										// between +/- ]MAX_YEARS/2[.
	int dst(int i = -1); // Please refer to tm.is_dst documentation.

#if DATE_MIC_ON == 1
	// Returns MicroSecond or throws WRONG_MS.
	constexpr static MicroSeconds MakeMS(int val)
	{	return val < MS_MAX ? val : throw DateError::WRONG_MS; }
#endif


//-------------------------------------------------- Operator overloadings

	// Classic assignment operator.
	Date& operator= (const Date&) = default;

	// Comparison operators.
	inline bool operator== (const Date& b) const {
#if DATE_MS_ON == 1
		if (timet == b.timet)
			return abs(b.microseconds - microseconds) <= tolerance;
		else
			return false;
#else
		return timet == b.timet;
#endif
	}

	inline bool operator!= (const Date& b) const { return !(*this == b); }
	inline bool operator<  (const Date& b) const { return Compare(b) == INFERIOR; }
	inline bool operator>  (const Date& b) const { return Compare(b) == SUPERIOR; }
	inline bool operator<= (const Date& b) const { return Compare(b) != SUPERIOR; }
	inline bool operator>= (const Date& b) const { return Compare(b) != INFERIOR; }

	// Returns interval.
	inline Interval operator% (const Date& b) const { return Timedelta(b); }

	// Arithmetics
	Date operator+ (Interval seconds) const;
	Date operator- (Interval seconds) const;
	Date& operator+= (Interval seconds);
	Date& operator-= (Interval seconds);

	// Conversions
	inline operator struct tm() const { return time; }
	inline operator time_t () const { return timet; }
	operator std::string() const;

//---------------------------------------------- Constructors - destructor
    
    Date(const Date&) = default;

#if DATE_MIC_ON == 1
	explicit Date(tm, MicroSeconds = 0);
	explicit Date(time_t, MicroSeconds = 0);
	explicit Date(const std::string&, const char* pattern, MicroSeconds = 0);
#else
	Date(tm);
	Date(time_t);
	Date(const std::string&, const char* pattern);
#endif

	Date();
	virtual ~Date() = default;

//---------------------------------------------------------------- PRIVATE

protected:
//------------------------------------------------------ Protected methods
	
	// If newvalue < max: sets field then returns it.
	// If newvalue == -1: returns field.
	// Else throw DateError::WRONG_TIME_DATA.
	int& quickSetter(int newvalue, int max, int& field);
//--------------------------------------------------- Protected attributes

	tm time; // tm struct that holds the current date.
	time_t timet; // time_t that holds the current date.
	
	// Use it when converting with string fields.
	// It will be used the way strftime does.
	// Don't include microseconds here.
	static const char* pattern; 
	
	// Constants for comparison results.
	static const int INFERIOR, SUPERIOR, EQUAL; 

	const static int MAX_YEARS;
	
#if DATE_MIC_ON == 1
	// Field that holds microseconds.
	MicroSeconds microseconds;

	// Tolerance in microseconds. Initial value of MS_MAX.
	static MicroSeconds tolerance; 
	
	constexpr static MicroSeconds MS_MAX = MicroSeconds(1e6); 

	// Character used to separate the datetime to the microseconds
	// in the string representations.
	// NO_MS is default value.
	static char msSep;

	constexpr static char NO_MS = 0x00; // Do not represent microseconds in string.
#endif
};

//------------------------------------------------------ Other definitions

#endif // DATE_H
