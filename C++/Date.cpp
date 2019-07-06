//---------- Implementation of class <Date> (file Date.cpp) 

//---------------------------------------------------------------- INCLUDE

//-------------------------------------------------------- System includes

//------------------------------------------------------ Personal includes
#include "Date.h"

//-------------------------------------------------------------- Constants
int Date::SUPERIOR = 1;
int Date::INFERIOR = -1;
int Date::EQUAL = 0;

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods
int Date::Compare(const Date& d) const 
{
	return EQUAL; 
}

unsigned int Date::Timedelta(const Date&) const 
{
	return -1; 
}
//-------------------------------------------------- Operator overloadings
bool Date::operator== (const Date& d) const
{
	return Compare(d) == EQUAL;
}

bool Date::operator!= (const Date& d) const
{
	return Compare(d) != EQUAL;
}

bool Date::operator< (const Date& d) const
{
	return Compare(d) == INFERIOR;
}

bool Date::operator> (const Date& d) const
{
	return Compare(d) == SUPERIOR;
}

bool Date::operator<= (const Date& d) const
{
	return Compare(d) != SUPERIOR;
}

bool Date::operator>= (const Date& d) const
{
	return Compare(d) != INFERIOR;
}

Date Date::operator+ (const Date& d) const
{
	return Date();
}

Date Date::operator- (const Date& d) const
{
	return Date();
}

unsigned int Date::operator% (const Date& d) const
{
	return Timedelta(d);
}

Date& Date::operator+= (const Date& d)
{
	return *this;
}

Date& Date::operator-= (const Date& d)
{
	return *this;
}

Date::operator struct tm() const
{
	return time;
}

Date::operator time_t () //const
{
	return mktime(&time);
}

Date::operator std::string() const
{
	return "";
}


//---------------------------------------------- Constructors - destructor
Date::Date() :
	time(),
	microseconds()
{
}

Date::Date(tm time, unsigned int ms)
{
	if (mktime(&time) == time_t(-1) || ms > 999999)
		throw nullptr;

	this->time = time;
	microseconds = ms;
}

Date::Date(time_t tmt, unsigned int ms)
{

}

Date::Date(const std::string&, const char* pattern)
{

}
	
//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods
