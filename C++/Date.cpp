//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE
#define _Import_Date_Please
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cassert>
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

const int DAYS_PER_MON [] = { 
	31, // jan 
	28, // feb
	31, // mar
	30, // apr
	31, // may
	30, // jun
	31, // jul
	31, // aug
	30, // sep
	31, // oct
	30, // nov
	31  // dec
};

constexpr auto NBR_SECONDS_IN_DAY = 24 * 60 * 60;

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods
#if DATE_MIC_ON == 1
int Date::Compare(const Date& d, bool compareMS) const 
{
	int res;
	double distance = Timedelta(d);
	if (compareMS || abs(distance) >= 1)
	{
		res = Toolbox::Sign(0);
	}
	else
		res = Toolbox::Sign(int(distance));
	return res;
} // TODO
#else
inline int Date::Compare(const Date& d) const
{
	return Toolbox::Sign(int(Timedelta(d)));
}
#endif

inline Interval Date::Timedelta(const Date& param) const
{
	return Interval(difftime(timet, param.timet)
#if DATE_MIC_ON == 1
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

////Date Date::operator+ (const Date& d) const
////{
////	Date newdate(*this);
////	newdate += d;
////	return newdate;
////}
////
////Date Date::operator- (const Date& d) const
////{
////	Date newdate(*this);
////	newdate -= d;
////	return newdate;
////}

inline Interval Date::operator% (const Date& d) const
{
	return Timedelta(d);
}

////Date& Date::operator+= (const Date& d)
////{
////#if DATE_MIC_ON == 1
////	microseconds += d.microseconds;
////	int leapSeconds = microseconds / MS_MAX;
////	timet += leapSeconds;
////	microseconds %= MS_MAX;
////#endif
////	timet += d.timet;
////	localtime_s(&time, &timet);
////	return *this;
////} // TODO
////
////Date& Date::operator-= (const Date& d)
////{
////#if DATE_MIC_ON == 1
////	microseconds -= d.microseconds;
////	int leapSeconds = microseconds / MS_MAX;
////	timet -= leapSeconds;
////	microseconds %= MS_MAX;
////#endif
////	timet += d.timet;
////	localtime_s(&time, &timet);
////	return *this;
////} // TODO

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

#if DATE_MIC_ON == 1
	if (msSep != NO_MS)
		oss << msSep << microseconds;
#endif

	return oss.str();
}


//---------------------------------------------- Constructors - destructor
#if DATE_MIC_ON == 1
Date::Date() :
	time({ 0 }),
	microseconds(0),
	timet(0)
{
}

Date::Date(tm time_in, MicroSeconds ms) :
	microseconds(makeMS(ms)),
	timet(mktime(&time_in)),
	time(time_in)
{
	if (timet == time_t(-1) || ms >= MS_MAX)
		throw DateError::WRONG_STRUCT_TM;
}

Date::Date(time_t tmt, MicroSeconds ms) :
	microseconds(makeMS(ms)),
	timet(tmt)
{
	if (localtime_s(&time, &tmt))
		throw DateError::WRONG_TIME_T;
}

Date::Date(const std::string& src, const char* pattern, MicroSeconds ms) :
	microseconds(makeMS(ms))
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

	if (timet = mktime(&time) == time_t(-1)) {
		throw DateError::WRONG_STRING;
	}
}
#else
Date::Date() :
	time({ 0 }),
	timet(0)
{
}

Date::Date(tm time_in) :
	timet(mktime(&time_in)),
	time(time_in)
{
	if (timet == time_t(-1))
		throw DateError::WRONG_STRUCT_TM;
}

Date::Date(time_t tmt) :
	timet(tmt)
{
	if (localtime_s(&time, &tmt))
		throw DateError::WRONG_TIME_T;
}

Date::Date(const std::string& src, const char* pattern)
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

	if (timet = mktime(&time) == time_t(-1)) {
		throw DateError::WRONG_STRING;
	}
}
#endif
	
//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods
#if DATE_MIC_ON == 1
MicroSeconds Date::makeMS(MicroSeconds ms)
{
	if (ms > MS_MAX)
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
