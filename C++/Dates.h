//---------- Interface of module <Dates> (file Dates.h) 
#if ! defined ( DATES_H )
#define DATES_H

//--------------------------------------------------------------- Includes
#include <ctime>
#include <string>

typedef tm Date;

// Returns the number of seconds between both dates.
int operator- (Date a, Date b);

// Adds or removes some seconds to the given date.
// Parameter a is given by copy!
Date operator+(Date a, int seconds); Date operator+(int seconds, const Date& a);

// Strict equality operator.
bool operator== (const Date& a, const Date& b);

// Comparison required by ordered map.
bool operator< (const Date& a, const Date& b);

namespace Business
{
	// Role of Dates:
	// Contains conversion and comparison tools for dates.
	namespace Dates 
	{
//-------------------------------------------------------------- Constants
		const int INFERIOR = -1;
		const int EQUAL = 0;
		const int SUPERIOR = +1;
//------------------------------------------------------------------ Types
		
		// Enum for used patterns.
		enum DatePattern_e 
		{
			A, // "YYYY-MM-ddTHH:mm:ss" where T represents nothing.
			B // "YYYY/MM/dd HH:mm"
		};
//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions
		
		// Converts the given date to its string representation.
		// The patternID specifies the shape of the final string.
		// Inputs: any date, any valid patternID.
		// Warnings: an invalid patternID raises a DateException.
		std::string Convert(const Date& d, DatePattern_e = A);

		// Attempts to convert a string to a date.
		// Inputs: any string.
		// Warnings: if conversion fails, throws a DateException.
		Date Convert(const std::string& d);

		// Compares both dates.
		// Returns INFERIOR if d1<d2, EQUAL if d1==d2 or SUPERIOR if d1>d2.
		// Inputs: two dates in any order, and a tolerance number
		// the max seconds gap between both to be considered equal.
		// If not positive, the function does tol=-tol.
		// If both dates are distant of 'tol' seconds or less, returns EQUAL.
		int Compare(const Date& d1, const Date& d2, int tol = 60);

		
	}
}

#endif // DATES_H
