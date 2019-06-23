//---------- Interface of module <File> (file File.h) 
#if ! defined ( FILE_H )
#define FILE_H

//--------------------------------------------------------------- Includes
#include <fstream>
#include <string>

// Role of File:
//	Encapsulates the opening of a file input stream.
namespace SystemIO
{

	// Type used to deal with file sizes of any weight (GBs are okay).
	typedef unsigned long filesize_t;

	namespace File
	{
//-------------------------------------------------------------- Constants
		const std::string ENC_UTF16LE = "UTF-16LE";
		const std::string ENC_UTF8 = "UTF-8";
		const std::string ENC_DEFAULT = "DEFAULT";
//------------------------------------------------------------------ Types
		

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

		// Description: Opens given file with the given stream.
		// Inputs: any ifstreal, any filename.
		// Warnings: if file cannot be openend for reading, is empty,
		//	or is ".", throws a FileException.
		//	If input encoding is not known, opens with UTF-8 (risky).
		void Open(std::ifstream& ifs, const std::string& fileName);

		// Description: Returns file size.
		// Inputs: a valid filename.
		// Warnings: if anything fails, throws a FileException.
		filesize_t Size(const std::string& filename);

		// Returns the encoding of the file as one of the strings declared above.
		// Warnings: If file is empty, throws a FileException.
		//	If no encoding is found, returns "".
		std::string Encoding(const std::string& filename);
	} 
}

#endif // FILE_H
