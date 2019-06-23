//---------- Interface of class <BusinessFile> (file BusinessFile.h) ----------------
#if ! defined ( BUSINESSFILE_H )
#define BUSINESSFILE_H

//--------------------------------------------------------------- Includes
#include "CSVParser.h"

namespace SystemIO
{
//------------------------------------------------------- Public constants

//------------------------------------------------------------------ Types

	// Role of BusinessFile: 
	// 	Abstract class for getting CSV files' content.
	//	The constructor is protected.
	class BusinessFile
	{
//----------------------------------------------------------------- PUBLIC

	public:
//--------------------------------------------------------- Public methods

		// Makes the parser open the file by calling File::Open.
		// In implementations, it also check that the first line
		// is correct with the associated Business type.
		virtual void Open(const std::string& filename);

		// Changes the CSV separator character (default is in CSVParser.h).
		// The given char must be printable, otherwise it is not applied.
		// Returns the separator at end of call.
		virtual char Separator(char newSep = '\t');

//-------------------------------------------------- Operator overloadings

		// NO assignment operator.
		BusinessFile& operator = (const BusinessFile& a) = delete;

//---------------------------------------------- Constructors - destructor
    
    	// NO copy constructor.
		BusinessFile(const BusinessFile& a) = delete;

	    // NO default constructor.
		BusinessFile() = delete;

	    // Classic destructor.
		virtual ~BusinessFile();

//---------------------------------------------------------------- PRIVATE

	protected:
//------------------------------------------------------ Protected methods
		
		typedef BusinessFile super; // Used by derived classes.

		// Default constructor. Initializes the parser and number of seps.
		// Protected because the class is only relevant for inheritance.
		// Inputs: number of separators, of elements in a normal line.
		BusinessFile(short num);
//--------------------------------------------------- Protected attributes
		CSVParser parser; // Parser to be used for parsing files.

		// Normally it is the first line of the file.
		// It must be manually set by heirs.
		std::vector<std::string> firstLine; 

		short numOfSeparators; // Number of separators.
	};

//------------------------------------------------------ Other definitions

}

#endif // BUSINESSFILE_H
