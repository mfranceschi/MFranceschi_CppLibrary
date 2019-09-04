//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE

#include <cassert>
#include <chrono>
#include <cmath>
#include <limits>
#include <iomanip>
#include <sstream>
#include "Date.hpp"
#include "Toolbox.hpp"

//-------------------------------------------------------------- Constants

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
	#define _Constr_Param_Microseconds , MicroSeconds ms
	#define _Constr_Init_List_Microseconds , microseconds_in(MakeMS(ms))
#else
	#define _Constr_Param_Microseconds /* Nothing */
	#define _Constr_Init_List_Microseconds /* Nothing */
#endif

constexpr static int MaxYear()
{
	typedef unsigned long long ULL_t;
	constexpr ULL_t seconds_in_year = ULL_t(Date::NBR_SECONDS_IN_DAY * 365.2425L);
	constexpr ULL_t possible_seconds(std::numeric_limits<time_t>::max());
	constexpr ULL_t max_year_with_time_t(possible_seconds / seconds_in_year);
	constexpr int max_year_with_struct_tm(std::numeric_limits<int>::max());

	if (max_year_with_time_t> max_year_with_struct_tm)
		return max_year_with_struct_tm;
	else
		return int(max_year_with_time_t);
}

const int Date::MAX_YEAR = MaxYear();

// Executes the localtime function and returns true if success.
inline bool Localtime_Func(const time_t& source, struct tm& result)
{
#ifdef _WIN32
	/* Win32-style Localtime_S is available. */
	return !localtime_s(&result, &source);

#elif defined __STDC_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1
	/* Classic Localtime_S is available. */
	return localtime_s(&source, &result);
#else
	/* Use POSIX-style Localtime_R. */
	return localtime_r(&source, &result);

	/* 
	 * To use basic "localtime", we should use mutexes to ensure there are no conflicts. 
	 * I am not implementing it here. 
	 */
#endif
}

#define ASSERT_OK assert((timet = mktime(&time)) != -1); /* Ensures the instance is in a valid state or fail assertion. */

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods

const char* Date::str_pattern(const char* pattern)
{
	if (pattern == nullptr || strlen(pattern) != 0)
	{
		return Date::pattern = pattern;
	}
	else
		return Date::pattern;
}

int Date::Compare(const Date& d) const
{
#ifdef _MicroSeconds
	return Toolbox::Sign(Timedelta(d));
#else
	return Toolbox::Sign(trunc(Timedelta(d)));
#endif
}

Interval Date::Timedelta(const Date& param) const
{
	Interval result = static_cast<Interval>(difftime(timet, param.timet));
#ifdef _MicroSeconds
	result += static_cast<Interval>(double(microseconds_in - param.microseconds_in) / double(MS_MAX));
#endif
	return result;
}

// Getters and setters.

int Date::seconds(int newvalue)
{
	return quickSetter(newvalue, 60+1, time.tm_sec);
}

int Date::minutes(int newvalue)
{
	return quickSetter(newvalue, 60, time.tm_min);
}

int Date::hours(int newvalue)
{
	return quickSetter(newvalue, 24, time.tm_hour);
}

int Date::day_month(int newvalue)
{
	if (newvalue >= 1)
	{
		int daysInMonth = 31;
		if (time.tm_mon == APRIL ||
			time.tm_mon == JUNE ||
			time.tm_mon == SEPTEMBER ||
			time.tm_mon == NOVEMBER
		)
			daysInMonth = 30;
		else if (time.tm_mon == FEBRUARY)
		{
			if (IsLeapYear(time.tm_year - 1900))
				daysInMonth = 29;
			else
				daysInMonth = 28;
		}

		time.tm_mday = newvalue;
		ASSERT_OK;
		return time.tm_mday;
	}
	else if (newvalue == -1)
		return time.tm_mday;
	else
		throw DateError::WRONG_TIME_DATA;
}

int Date::month(int newvalue)
{
	return quickSetter(newvalue, 12, time.tm_mon);
}

int Date::year(int newvalue)
{
	if (newvalue < MAX_YEAR && newvalue > -MAX_YEAR-1)
	{
		time.tm_year = newvalue;
		ASSERT_OK;
		return time.tm_year;
	}
	else
		throw DateError::WRONG_TIME_DATA;
} // TODO

int Date::dst(int newvalue)
{
	if (newvalue == -1 || newvalue == 0 || newvalue == 1)
	{
		time.tm_isdst = newvalue;
		ASSERT_OK;
		return time.tm_isdst;
	}
	else
		throw DateError::WRONG_TIME_DATA;
}

#ifdef _MicroSeconds
MicroSeconds Date::microseconds(MicroSeconds newms)
{
	if (newms == -1)
		return microseconds_in;
	else
		return microseconds_in = MakeMS(newms);
}

MicroSeconds Date::ms_tolerance(MicroSeconds newms)
{
	if (newms == -1)
		return tolerance;
	else
		return tolerance = MakeMS(newms);
}
#endif

//-------------------------------------------------- Operator overloadings
bool Date::operator== (const Date& b) const {
#ifdef _MicroSeconds
	if (timet == b.timet)
		return abs(static_cast<int>(b.microseconds_in) - static_cast<int>(microseconds_in)) <= static_cast<int>(tolerance);
	else
		return false;
#else
	return timet == b.timet;
#endif
}

Date::operator std::string() const
{
	if (pattern == nullptr) 
		throw DateError::NO_PATTERN;

	std::ostringstream oss;
	oss << std::put_time(&time, pattern);

#ifdef _MicroSeconds
	if (msSep != NO_MS)
		oss << msSep << microseconds_in;
#endif

	return oss.str();
}


//---------------------------------------------- Constructors - destructor

Date::Date()
{
	using namespace std::chrono;
	using clock = system_clock;
	clock::time_point now_point = clock::now();
	timet = clock::to_time_t(now_point);
	ASSERT_OK; /* Also sets this->time. */
#ifdef _MicroSeconds
	using mics = std::chrono::microseconds;
	mics::rep current_microseconds_count = duration_cast<mics>(now_point.time_since_epoch()).count();
	microseconds_in = static_cast<MicroSeconds>(current_microseconds_count - 1000000 * timet);
#endif
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
	if (!Localtime_Func(timet, time))
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
		ASSERT_OK;
		return field;
	}
	else if (newvalue == int(-1))
		return field;
	else
		throw DateError::WRONG_TIME_DATA;
}
