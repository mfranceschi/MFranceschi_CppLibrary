//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE

//-------------------------------------------------------- System includes

//------------------------------------------------------ Personal includes
#include <iomanip>
#include <sstream>
#include "Date.h"

//-------------------------------------------------------------- Constants
const int Date::SUPERIOR = 1;
const int Date::INFERIOR = -1;
const int Date::EQUAL = 0;
const char* Date::pattern = nullptr;
const MicroSeconds Date::MS_MAX = MicroSeconds(1000000);
MicroSeconds Date::tolerance = Date::MS_MAX;
char Date::msSep = NO_MS;

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods
int Date::Compare(const Date& d, bool compareMS) const 
{
	if (*this == d)
		return EQUAL;

	auto distance = Timedelta(d);
	// Smtg to consider whether or not look at microseconds
	// if else
	return 0;
} // TODO

double Date::Timedelta(const Date& param) const 
{
	return difftime(timet, param.timet) + 
		double(microseconds - param.microseconds) / double(MS_MAX);
}

//-------------------------------------------------- Operator overloadings
inline bool Date::operator== (const Date& b) const
{
	if (timet == b.timet)
		return abs(b.microseconds - microseconds) <= tolerance;
	else 
		return false;
}

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

Date Date::operator+ (const Date& d) const
{
	Date newdate(*this);
	newdate += d;
	return newdate;
}

Date Date::operator- (const Date& d) const
{
	Date newdate(*this);
	newdate -= d;
	return newdate;
}

inline double Date::operator% (const Date& d) const
{
	return Timedelta(d);
}

Date& Date::operator+= (const Date& d)
{
	return *this;
} // TODO

Date& Date::operator-= (const Date& d)
{
	return *this;
} // TODO

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

	if (msSep != NO_MS)
		oss << msSep << microseconds;

	return oss.str();
}


//---------------------------------------------- Constructors - destructor
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

	time = time_in;
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
	
//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods
MicroSeconds Date::makeMS(MicroSeconds ms)
{
	if (ms > MS_MAX)
		throw DateError::WRONG_MS;
	else
		return ms;
}
