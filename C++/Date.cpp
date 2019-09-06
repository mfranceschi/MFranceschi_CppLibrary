//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <functional>
#include <iomanip>
#include <limits>
#include <mutex>
#include <sstream>
#include "Date.hpp"
#include "Toolbox.hpp"

#ifdef max
#undef max
#endif // max

//-------------------------------------------------------------- Constants

const char* Date::pattern = nullptr;

#if ! defined DATE_MIC_ON || !(DATE_MIC_ON == 1 || DATE_MIC_ON == 0)
#error "The macro DATE_MIC_ON must be defined !"
#endif

#if DATE_MIC_ON == 1
MicroSeconds Date::tolerance = Date::MS_MAX;
char Date::msSepChar = NO_MS;
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
#pragma warning(disable: 4127 4309 4365)
{
	typedef unsigned long long ULL_t;
	constexpr ULL_t seconds_in_year = ULL_t(Date::NBR_SECONDS_IN_DAY * 365.2425L);
	constexpr ULL_t possible_seconds(std::numeric_limits<time_t>::max());
	constexpr ULL_t max_year_with_time_t(possible_seconds / seconds_in_year);
	constexpr int max_year_with_struct_tm(std::numeric_limits<int>::max());

	if (max_year_with_time_t > max_year_with_struct_tm)
		return static_cast<int>(max_year_with_struct_tm);
	else
		return static_cast<int>(max_year_with_time_t);
}

const int Date::MAX_YEAR = MaxYear();
#pragma warning(default: 4127 4309 4365)

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

// Mutexes for static variables access.
static std::mutex tolerance_mutex, pattern_mutex, msSep_mutex;

// Template function for getter and setter of static variables access.
template <typename T>
static T set_static_value(T newvalue, std::mutex& mutex_i, T& stored_value, std::function<bool(T)> isReadOnly)
{
	mutex_i.lock();
	T returnValue = isReadOnly(newvalue) ? stored_value : (stored_value = newvalue);
	mutex_i.unlock();
	return returnValue;
}

#define ASSERT_OK assert((timet = mktime(&time)) != -1); /* Ensures the instance is in a valid state or fail assertion. */

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods

const char* Date::str_pattern(const char* npattern)
{
	return set_static_value<decltype(Date::pattern)>(npattern, pattern_mutex, Date::pattern, [](const char* newp) {
		return newp != nullptr && strlen(newp) == 0;
		});
}

char Date::ms_CharSep(char newsep)
{
	return set_static_value<char>(newsep, msSep_mutex, Date::msSepChar, [](char newc) {
		return newc == '\r';
		});
}

int Date::DaysInMonth(int month, int year) noexcept
{
	if (month >= 0 && month < 12)
	{
		int daysInMonth = 31;
		if (month == APRIL ||
			month == JUNE ||
			month == SEPTEMBER ||
			month == NOVEMBER
			)
			daysInMonth = 30;
		else if (month == FEBRUARY)
		{
			if (IsLeapYear(year))
				daysInMonth = 29;
			else
				daysInMonth = 28;
		}
		return daysInMonth;
	}
	else return 0;
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
	if (!newvalue) 
		newvalue = -2;
	return quickSetter(newvalue, Date::DaysInMonth(newvalue, time.tm_year - 1900) + 1, time.tm_mday);
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
	if (newms == MicroSeconds(-1))
		return microseconds_in;
	else
		return microseconds_in = MakeMS(newms);
}

MicroSeconds Date::ms_tolerance(MicroSeconds newms)
{
	return set_static_value<MicroSeconds>(newms, tolerance_mutex, Date::tolerance, [](MicroSeconds ms) {
		return ms == MicroSeconds(-1);
		});
}
#endif

//-------------------------------------------------- Operator overloadings
bool Date::operator== (const Date& b) const {
#ifdef _MicroSeconds
	using temp_type = long long int;
	temp_type current = temp_type(timet) * MS_MAX + microseconds_in;
	temp_type other = temp_type(b.timet) * MS_MAX + microseconds_in;
	return abs(current - other) <= temp_type(tolerance);
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
	if (msSepChar != NO_MS)
		oss << msSepChar << microseconds_in;
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
	time(time_in),
	timet(mktime(&time))

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

Date::Date(const std::string& src, const char* npattern _Constr_Param_Microseconds) :
	timet(0) /* sorry about that :) */
	_Constr_Init_List_Microseconds
{
	if (npattern == nullptr)
	{
		if (Date::pattern != nullptr)
			npattern = Date::pattern;
		else
			throw DateError::NO_PATTERN;
	}

	std::istringstream iss(src);
	Date ans;
	iss >> std::get_time(&time, npattern);

	if ((timet = mktime(&time)) == time_t(-1)) {
		throw DateError::WRONG_STRING;
	}
}
	
//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods

int Date::quickSetter(int newvalue, int max, int& field)
{
	if (newvalue < max && newvalue >= 0)
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
