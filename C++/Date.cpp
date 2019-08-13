//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE

#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "Date.h"
#include "Toolbox.h"

//-------------------------------------------------------------- Constants

const int Date::SUPERIOR = 1;
const int Date::INFERIOR = -1;
const int Date::EQUAL = 0;
const char* Date::pattern = nullptr;

#if ! defined DATE_MIC_ON || !(DATE_MIC_ON == 1 || DATE_MIC_ON == 0)
#error "The macro DATE_MIC_ON must be defined !"
#endif

#if DATE_MIC_ON == 1
MicroSeconds Date::tolerance = Date::MS_MAX;
char Date::msSep = NO_MS;
#define _MicroSeconds 1 /* Convenience solution for more readable code. */
#endif

#ifdef _MicroSeconds
	#define _Inline_If_No_Microseconds /* Nothing */
	#define _Constr_Param_Microseconds , MicroSeconds ms
	#define _Constr_Init_List_Microseconds , microseconds(makeMS(ms))
	#define _Constr_Init_List_Default_Microseconds , microseconds(0)
#else
	#define _Inline_If_No_Microseconds inline
	#define _Constr_Param_Microseconds
	#define _Constr_Init_List_Microseconds
	#define _Constr_Init_List_Default_Microseconds
#endif

constexpr int JANUARY = 0;
constexpr int FEBRUARY = 1;
constexpr int MARCH = 2;
constexpr int APRIL = 3;
constexpr int MAY = 4;
constexpr int JUNE = 5;
constexpr int JULY = 6;
constexpr int AUGUST = 7;
constexpr int SEPTEMBER = 8;
constexpr int OCTOBER = 9;
constexpr int NOVEMBER = 10;
constexpr int DECEMBER = 11;

// First line = classic years. Second line = more rare years.
const int DAYS_PER_MON [2][12] = 
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

constexpr auto NBR_SECONDS_IN_DAY = 24 * 60 * 60;

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods

_Inline_If_No_Microseconds int Date::Compare(const Date& d) const
{
#ifdef _Microseconds
	return Toolbox::Sign(Timedelta(d));
#else
	int res;
	Interval distance = Timedelta(d);
	if (abs(distance) <= 1)
		res = Date::EQUAL;
	else
		res = Toolbox::Sign(distance);
	return res;
#endif
}

_Inline_If_No_Microseconds Interval Date::Timedelta(const Date& param) const
{
	return Interval(difftime(timet, param.timet)
#ifdef _Microseconds
		+ double(microseconds - param.microseconds) / double(MS_MAX)
#endif
	);
}

// Getters and setters.

int Date::seconds(__uv newvalue)
{
	return quickSetter(newvalue, 60, time.tm_sec);
}

int Date::minutes(__uv newvalue)
{
	return quickSetter(newvalue, 60, time.tm_min);
}

int Date::hours(__uv newvalue)
{
	return quickSetter(newvalue, 24, time.tm_hour);
}

int Date::monthday(__uv newvalue)
{
	int daysInMonth;
	if (time.tm_mon == APRIL || 
		time.tm_mon == JUNE || 
		time.tm_mon == SEPTEMBER || 
		time.tm_mon == NOVEMBER
	)
		daysInMonth = 30;
	else if (time.tm_mon == FEBRUARY)
	{
		int year = time.tm_year - 1900;
		// Code snippet from 
		// https://www.tutorialspoint.com/cplusplus-program-to-check-leap-year
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
			daysInMonth = 29;
		else
			daysInMonth = 28;
	}
	else
		daysInMonth = 31;

	if (newvalue >= 1)
	{

		time.tm_mday = newvalue;
		assert(timet = mktime(&time) != -1);
		return time.tm_mday;
	}
	else if (newvalue == __uv(-1))
		return time.tm_mday;
	else
		throw DateError::WRONG_TIME_DATA;
}

int Date::month(__uv newvalue)
{
	return quickSetter(newvalue, 12, time.tm_mon);
}

int Date::weekday(__uv newvalue)
{
	return -1;
} // TODO

int Date::yearday(__uv newvalue)
{
	return -1;
} // TODO

int Date::year(int newvalue)
{
	if (newvalue < MAX_YEARS/2 && newvalue > -MAX_YEARS/2)
	{
		time.tm_year = newvalue;
		assert(timet = mktime(&time) != -1);
		return time.tm_year;
	}
	else if (newvalue == __uv(-1))
		return time.tm_year;
	else
		throw DateError::WRONG_TIME_DATA;
}

int Date::dst(int newvalue)
{
	if (newvalue == -1 || newvalue == 0 || newvalue == 1)
	{
		time.tm_isdst = newvalue;
		assert(timet = mktime(&time) != -1);
		return time.tm_isdst;
	}
	else if (newvalue == -2)
		return time.tm_isdst;
	else
		throw DateError::WRONG_TIME_DATA;
}

//-------------------------------------------------- Operator overloadings
#if DATE_MS_ON == 1
inline bool Date::operator== (const Date& b) const
{
	if (timet == b.timet)
		return abs(b.microseconds - microseconds) <= tolerance;
	else 
		return false;
}
#else
inline bool Date::operator== (const Date& b) const
{
	return timet == b.timet;
}
#endif

inline bool Date::operator!= (const Date& d) const
{
	return !(*this == d);
}

inline bool Date::operator< (const Date& d) const
{
	return Compare(d) == INFERIOR;
}

inline bool Date::operator> (const Date& d) const
{
	return Compare(d) == SUPERIOR;
}

inline bool Date::operator<= (const Date& d) const
{
	return Compare(d) != SUPERIOR;
}

inline bool Date::operator>= (const Date& d) const
{
	return Compare(d) != INFERIOR;
}

inline Interval Date::operator% (const Date& d) const
{
	return Timedelta(d);
}

inline Date::operator struct tm() const
{
	return time;
}

inline Date::operator time_t () const
{
	return timet;
}

Date::operator std::string() const
{
	if (pattern == nullptr) 
		throw DateError::NO_PATTERN;

	std::ostringstream oss;
	oss << std::put_time(&time, pattern);

#ifdef _MicroSeconds
	if (msSep != NO_MS)
		oss << msSep << microseconds;
#endif

	return oss.str();
}


//---------------------------------------------- Constructors - destructor

Date::Date() :
	time({ 0 }),
	timet(0)
	_Constr_Init_List_Default_Microseconds
{
}

Date::Date(tm time_in _Constr_Param_Microseconds) :
	timet(mktime(&time_in)),
	time(time_in)
	_Constr_Init_List_Microseconds
{
	if (timet == time_t(-1))
		throw DateError::WRONG_STRUCT_TM;
}

Date::Date(time_t tmt _Constr_Param_Microseconds) :
	timet(tmt)
	_Constr_Init_List_Microseconds
{
	if (localtime_s(&time, &tmt))
		throw DateError::WRONG_TIME_T;
}

Date::Date(const std::string& src, const char* pattern _Constr_Param_Microseconds) :
	timet(0) /* sorry about that :) */
	_Constr_Init_List_Microseconds
{
	if (pattern == nullptr)
	{
		if (Date::pattern != nullptr)
			pattern = Date::pattern;
		else
			throw DateError::NO_PATTERN;
	}

	std::istringstream iss(src);
	Date ans;
	iss >> std::get_time(&time, pattern);

	if ((timet = mktime(&time)) == time_t(-1)) {
		throw DateError::WRONG_STRING;
	}
}
	
//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods
#if DATE_MIC_ON == 1
MicroSeconds Date::makeMS(MicroSeconds ms)
{
	if (ms >= MS_MAX)
		throw DateError::WRONG_MS;
	else
		return ms;
}
#endif

int& Date::quickSetter(__uv newvalue, __uv max, int& field)
{
	if (newvalue < max)
	{
		field = newvalue;
		assert(timet = mktime(&time) != -1);
		return field;
	}
	else if (newvalue == __uv(-1))
		return field;
	else
		throw DateError::WRONG_TIME_DATA;
}
