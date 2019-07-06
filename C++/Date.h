//---------- Interface of class <Date> (file Date.h) ----------------
#if ! defined ( DATE_H )
#define DATE_H

//--------------------------------------------------------------- Includes
#include <ctime>
#include <string>

//------------------------------------------------------- Public constants

//------------------------------------------------------------------ Types

// Role of Date: 
// Simple extension of C's struct tm with milliseconds and operator 
// overloadings. 
// Please note that the 'isdst', 'mday', 'wday' fields are ignored.
class Date
{
//----------------------------------------------------------------- PUBLIC

public:
//--------------------------------------------------------- Public methods
	int Compare(const Date&) const;
	unsigned int Timedelta(const Date&) const;

//-------------------------------------------------- Operator overloadings

	// Classic assignment operator.
	// Inputs: a good-formed Date.
	Date& operator= (const Date&) = default;

	bool operator== (const Date&) const;

	bool operator!= (const Date&) const;

	bool operator< (const Date&) const;

	bool operator> (const Date&) const;

	bool operator<= (const Date&) const;

	bool operator>= (const Date&) const;

	Date operator+ (const Date&) const;

	Date operator- (const Date&) const;

	unsigned int operator% (const Date&) const;

	Date& operator+= (const Date&);

	Date& operator-= (const Date&);

	operator struct tm() const;

	operator time_t () ;

	operator std::string() const;

//---------------------------------------------- Constructors - destructor
    
    // Classic copy constructor.
    // Inputs: a good-formed Date.
	Date(const Date&) = default;

	Date(tm, unsigned int microseconds = 0);

	Date(time_t, unsigned int microseconds = 0);

	Date(const std::string&, const char* pattern = nullptr);

	// Default constructor. 
	Date();

	// Classic destructor.
	virtual ~Date() = default;

//---------------------------------------------------------------- PRIVATE

protected:
//------------------------------------------------------ Protected methods

//--------------------------------------------------- Protected attributes
	tm time;
	int microseconds;
	static bool compareUnderSecond;
	static const char* pattern;
	static int INFERIOR, SUPERIOR, EQUAL;
};

//------------------------------------------------------ Other definitions

#endif // DATE_H
