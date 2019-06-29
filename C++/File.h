/* Apokk - MyWorks package - File module.
 
 * This module gives some basic informations about a given file.
 * 

 */

//---------- Interface of module <File> (file File.h) 
#if ! defined ( FILE_H )
#define FILE_H

//--------------------------------------------------------------- Includes
#include <fstream>
#include <string>

#ifndef FILESIZE_T
#define FILESIZE_T
// Type used to deal with file sizes of any weight (GBs are okay).
typedef unsigned long filesize_t
#endif // FILESIZE_T

namespace File
{
	typedef encoding_t std::string; // Represents an encoding.

//-------------------------------------------------------------- Constants
	const encoding_t ENC_UTF16LE = "UTF-16LE";
	const encoding_t ENC_UTF8 = "UTF-8";
	const encoding_t ENC_DEFAULT = "DEFAULT";
	const encoding_t ENC_UNKNOWN = "";
		

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	// Returns true if the file exists, false otherwise.
	// It is the fastest way I know for testing a file existence.
	bool Exists(const std::string& filename);

	// Returns true if trying to read charsToRead characters from the file fails.
	bool IsEmpty(const std::string& filename, size_t charsToRead = 0);
	
	// Closes ifs and tries to open filename according to Encoding's result.
	// Applies default locale if encoding is unknown.
	// To verify success, try ifs.is_open().
	// If an encoding is given, applies the corresponding locale.
	void Open(std::ifstream& ifs, const std::string& filename, const encoding_t given = ENC_UNKNOWN);

	// Description: Returns file size, or -1 in case of failure.
	// Inputs: a valid filename.
	filesize_t Size(const std::string& filename);

	// Returns the encoding of the file as one of the strings declared above.
	// It executes "IsEmpty(filename, 3)" firstly and if the result is true
	// returns "0".
	// If no encoding is found, returns "".
	encoding_t Encoding(const std::string& filename);
} 
#endif // FILE_H
