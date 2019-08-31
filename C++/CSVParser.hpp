//---------- Interface of class <CSVParser> (file CSVParser.h) ----------------
#if ! defined ( CSVPARSER_H )
#define CSVPARSER_H

//--------------------------------------------------------------- Includes
#include <fstream>
#include <vector>
#include "File.hpp"

namespace SystemIO
{
//------------------------------------------------------- Public constants

//------------------------------------------------------------------ Types

	// Role of CSVParser: 
	// 	Encapsulation of a file input stream for reading CSV files.
	//	Except for Open, all methods throw a FileException if the stream 
	//	is not open.
	//	If the stream is open and a FileException is thrown, 
	//	the CSVParser is back to his previous stable state.
	class CSVParser
	{
//----------------------------------------------------------------- PUBLIC

	public:
//--------------------------------------------------------- Public methods

		// Clears any error bit in the stream.
		void Clear();

		// Returns a list of string from the current line and goes to 
		// next line. 
		// Returns an empty vector if no separator is found.
		// Empty strings are not added to the vector.
		// Warnings: if anything fails, throws a FileException.
		std::vector<std::string> GetLine();

		// Goes to the beginning of the line in which the given position is.
		// Cannot go under the firstPos attribute.
		// Inputs: any position.
		// Warnings: if position exceed file size, throws a FileException.
		std::streampos Goto(std::streampos);

		// Moves the stream position of the given value.
		// Calls and returns Goto.
		std::streampos Move(std::streamoff);

		// Calls File::Open.
		// Please check File::Open for more details.
		void Open(const std::string&);

		// Returns the first element in the line before a separator.
		// The stream position is still at the beginning after that.
		// If nothing to read, returns an empty string.
		// Warnings: if reading fails, throws a FileException.
		std::string Peek();

		// Returns the streams current position.
		std::streampos Pos();

		// Goes to the previous line.
		// Calls and returns Goto.
		std::streampos PreviousLine();

		// Sets the separator if the given char is printable.
		// Returns the new separator.
		// Inputs: any character.
		char Separator(char = '\t');

		// Skips the current line and returns value of State().
		void Skip();

		// Returns file state after a reading attept.
		// The stream state may change but not its position.
		bool State();

//-------------------------------------------------- Operator overloadings

		// NO assignment operator.
		CSVParser& operator = (const CSVParser& unCSVParser) = delete;

		// Returns State().
		operator bool();

//---------------------------------------------- Constructors - destructor
    
    	// NO copy constructor.
		CSVParser(const CSVParser& unCSVParser) = delete;

	    // Default constructor. Nothing to initialize.
	    CSVParser ( );

	    // Classic destructor.
	    virtual ~CSVParser ( );

//---------------------------------------------------------------- PRIVATE

	protected:
//------------------------------------------------------ Protected methods
		
		// Clears the given string of non-print chars.
		// Returns a copy of the string.
		std::string sanitize(std::string& str);

//--------------------------------------------------- Protected attributes
		std::ifstream theStream; // File stream on the CSV.
		char separator; // Separator for CSV.
		char* buffer; // Bigger buffer for the stream.
		std::streampos firstPos; // Position of first interesting byte.
		filesize_t size; // Size of the openend file.
		std::string enc; // Encoding of the file.
	};

//------------------------------------------------------ Other definitions

}

#endif // CSVPARSER_H
