//---------- Interface of class <Date> (file Date.h) ----------------
#if ! defined ( DATE_H )
#define DATE_H

//--------------------------------------------------------------- Includes
#include <ctime>
#include <string>

//------------------------------------------------------- Public constants

//------------------------------------------------------------------ Types

// Type for representing microseconds.
typedef unsigned short MicroSeconds; 

// Error codes for this module.
enum DateError
{
	NO_PATTERN = 1,
	WRONG_STRUCT_TM = 2,
	WRONG_MS = 3,
	WRONG_STRING = 4,
	WRONG_TIME_T = 5
};

// Use this character to specify not to represent microseconds.
#define NO_MS char(0x00)

// Role of Date: 
// Simple extension of C's struct tm with milliseconds and operator 
// overloadings. 
// Please note that the 'isdst', 'mday' fields are ignored.
class Date
{
//----------------------------------------------------------------- PUBLIC

public:
//--------------------------------------------------------- Public methods

	// Compares the current date with the given one.
	// Returns Dates::INFERIOR if (this) < (param),
	// same idea for Dates::EQUAL and Dates::SUPERIOR.
	// The parameter 'compareMS' indicates whether we compare microseconds 
	// or not.
	int Compare(const Date&, bool compareMS = false) const;

	// Returns (this) - (param) in seconds.
	double Timedelta(const Date&) const;

//-------------------------------------------------- Operator overloadings

	// Classic assignment operator.
	Date& operator= (const Date&) = default;

	// Comparison operators.
	inline bool operator== (const Date&) const;
	inline bool operator!= (const Date&) const;
	inline bool operator< (const Date&) const;
	inline bool operator> (const Date&) const;
	inline bool operator<= (const Date&) const;
	inline bool operator>= (const Date&) const;

	// Returns (this) - (param) in seconds.
	inline double operator% (const Date&) const;

	// Add two dates
	Date operator+ (const Date&) const;

	Date operator- (const Date&) const;

	

	Date& operator+= (const Date&);

	Date& operator-= (const Date&);

	inline operator struct tm() const;

	inline operator time_t () const;

	operator std::string() const;

//---------------------------------------------- Constructors - destructor
    
    // Classic copy constructor.
    // Inputs: a good-formed Date.
	Date(const Date&) = default;

	Date(tm, MicroSeconds = 0);

	Date(time_t, MicroSeconds = 0);

	Date(const std::string&, const char* pattern = nullptr, MicroSeconds = 0);

	// Default constructor. 
	Date();

	// Classic destructor.
	virtual ~Date() = default;

//---------------------------------------------------------------- PRIVATE

protected:
//------------------------------------------------------ Protected methods
	
	// If value is correct, return it.
	// Else throw WRONG_MS.
	static MicroSeconds makeMS(MicroSeconds);
//--------------------------------------------------- Protected attributes

	tm time; // tm struct that holds the current date.
	time_t timet; // tm struct that holds the current date.
	
	// Field that holds microseconds.
	MicroSeconds microseconds; 
	
	// Use it when converting with string fields.
	// It will be used the way strftime does.
	static const char* pattern; 
	
	// Constants for comparison results.
	static const int INFERIOR, SUPERIOR, EQUAL; 
	
	// Tolerance in microseconds. Initial value of MS_MAX.
	static MicroSeconds tolerance; 
	
	// Max microseconds = 1M.
	static const MicroSeconds MS_MAX; 

	// Character used to separate the datetime to the microseconds
	// in the string representations.
	// NO_MS is default value.
	static char msSep;
};

//------------------------------------------------------ Other definitions

#endif // DATE_H
