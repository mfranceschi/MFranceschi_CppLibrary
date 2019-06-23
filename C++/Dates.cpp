//---------- Implementation of module <Dates> (file Dates.cpp) 

//--------------------------------------------------------------- Includes
#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <iomanip>
#include <ctime>
#include "../Globals.h"
#include "Dates.h"
#include <iostream>

using std::string; 
using std::istringstream;
using std::get_time;

namespace Business
{
	namespace Dates
	{
		
/////////////////////////////////////////////////////////////////  PRIVATE
		namespace
		{
//-------------------------------------------------------------- Constants
			const char* A_format = "%Y-%m-%dT%H:%M:%S";
//------------------------------------------------------------------ Types

//------------------------------------------------------- Static variables
			
//------------------------------------------------------ Private functions

		}

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions
		
		string Convert(const Date& d, DatePattern_e patternID)
		{
			const size_t TEMP_SIZE(256);
			char temp[TEMP_SIZE];
			const char** formatToApply;

			switch (patternID)
			{
			case A:
				formatToApply = &A_format;
				break;
			/*case B:
				formatToApply = &B_format;
				break;*/
			default:
				throw S_Exception(Err::UNKNOWN);
			}

			if (!strftime(temp, TEMP_SIZE, *formatToApply, &d))
			{
				throw S_Exception(Err::STR_D_FAIL);
			}
			return string(temp);
		}

		Date Convert(const string& d) 
		{
			istringstream iss(
				(d.find('.') == string::npos) ? 
				d : d.substr(0, d.find('.')));
			Date ans;
			iss >> get_time(&ans, A_format); 
			/*if (mktime(&ans) == time_t(-1)) {
				iss.seekg(0);
				/*std::istringstream iss(
					(d.find('.') == std::string::npos) ?
					d : d.substr(0, d.find('.')));
				iss >> get_time(&ans, B_format);
				
			}*/
			if (mktime(&ans) == time_t(-1)) {
				//throw S_Exception(Err::STR_D_FAIL);
			}
			return ans;
		}

		int Compare(const Date& d1, const Date& d2, int tol)
		{
			if (d1==d2) return EQUAL;
			
			if (tol < 0) tol = -tol;
			
			int ans = d1 - d2;
			if (abs(ans) <= tol)
				ans = EQUAL;
			else if (ans > 0)
				ans = SUPERIOR;
			else
				ans = INFERIOR;

			return ans;
		}



	}
}

int operator- (Date a, Date b)
{
	return int(difftime(mktime(&a), mktime(&b)));
}

Date operator+ (Date a, int seconds)
{
	time_t current = mktime(&a) + seconds;
	return *localtime(&current);
}

Date operator+ (int seconds, const Date& a)
{
	return operator+(a, seconds);
}

bool operator== (const Date& a, const Date& b)
{
	// It would be easier to compare time_t's but also a bit longer.
	return (
		a.tm_hour == b.tm_hour &&
		a.tm_mday == b.tm_mday &&
		a.tm_min == b.tm_min &&
		a.tm_mon == b.tm_mon &&
		a.tm_sec == b.tm_sec &&
		a.tm_year == b.tm_year
		);
}

bool operator< (const Date& a, const Date& b)
{
	return Business::Dates::Compare(a, b, 0) == Business::Dates::INFERIOR;
}