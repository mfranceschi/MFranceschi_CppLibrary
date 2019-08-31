//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE

#include <cassert>
#include <cmath>
#include <limits>
#include <iomanip>
#include <sstream>
#include "Date.hpp"
#include "Toolbox.hpp"

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
	#define _Constr_Init_List_Microseconds , microseconds(MakeMS(ms))
	#define _Constr_Init_List_Default_Microseconds , microseconds(0)
#else
	#define _Inline_If_No_Microseconds inline
	#define _Constr_Param_Microseconds /* Nothing */
	#define _Constr_Init_List_Microseconds /* Nothing */
	#define _Constr_Init_List_Default_Microseconds /* Nothing */
#endif

constexpr static int JANUARY = 0;
constexpr static int FEBRUARY = 1;
constexpr static int MARCH = 2;
constexpr static int APRIL = 3;
constexpr static int MAY = 4;
constexpr static int JUNE = 5;
constexpr static int JULY = 6;
constexpr static int AUGUST = 7;
constexpr static int SEPTEMBER = 8;
constexpr static int OCTOBER = 9;
constexpr static int NOVEMBER = 10;
constexpr static int DECEMBER = 11;

// First line = classic years. Second line = more rare years.
constexpr static int DAYS_PER_MON [2][12] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

constexpr static auto NBR_SECONDS_IN_DAY = 24 * 60 * 60;

constexpr static int MaxYears()
{
	typedef unsigned long long ULL_t;
	constexpr ULL_t seconds_in_year = ULL_t(60.0L * 60.0L * 24.0L * 365.2425L);
	constexpr ULL_t possible_seconds(std::numeric_limits<time_t>::max() - std::numeric_limits<time_t>::min() + ULL_t(1));
	constexpr ULL_t possible_years_with_big_int(possible_seconds / seconds_in_year);
	constexpr int max_year_with_struct_tm(std::numeric_limits<int>::max() - std::numeric_limits<int>::min() + ULL_t(1));

	if (possible_years_with_big_int > max_year_with_struct_tm)
		return max_year_with_struct_tm;
	else
		return int(possible_years_with_big_int);
}
const int Date::MAX_YEARS = MaxYears();

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

int Date::seconds(int newvalue)
{
	return quickSetter(newvalue, 60, time.tm_sec);
}

int Date::minutes(int newvalue)
{
	return quickSetter(newvalue, 60, time.tm_min);
}

int Date::hours(int newvalue)
{
	return quickSetter(newvalue, 24, time.tm_hour);
}

int Date::monthday(int newvalue)
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
		if (IsLeapYear(year))
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
	else if (newvalue == int(-1))
		return time.tm_mday;
	else
		throw DateError::WRONG_TIME_DATA;
}

int Date::month(int newvalue)
{
	return quickSetter(newvalue, 12, time.tm_mon);
}

int Date::weekday(int newvalue)
{
	return -1;
} // TODO

int Date::yearday(int newvalue)
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
	else if (newvalue == int(-1))
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

int& Date::quickSetter(int newvalue, int max, int& field)
{
	if (newvalue < max)
	{
		field = newvalue;
		assert(timet = mktime(&time) != -1);
		return field;
	}
	else if (newvalue == int(-1))
		return field;
	else
		throw DateError::WRONG_TIME_DATA;
}
