/* Martin Franceschi - MyWorks package - File module.
 * 
 * This module gives some basic informations about a given file.
 * You can check if it exists, check its size and encoding.
 * You can also open it by applying automatically a locale
 * (UTF-8 and UTF-16LE only).
 * All method are made so that it is the fastest possible way to achieve
 * these results.
 * For optimization purposes, the filename argument does not always have 
 * the same type for different functions.
 * 
 * In particular, the functions "Exists" and "Size" are as fast as possible.
 * 
 */

//---------- Interface of module <File> (file File.h) 
#if ! defined ( FILE_H )
#define FILE_H

//--------------------------------------------------------------- Includes
#include <fstream>
#include <string>

namespace File
{
	// Represents a file encoding.
	enum encoding_e {
		ENC_UTF16LE, // Normal UTF-16LE
		ENC_UTF8, // Normal UTF-8
		ENC_DEFAULT, // If no encoding is false, we assume the default locale.
		ENC_UNKNOWN // A problem occured while looking for the encoding.
	};

	// Represents a file encoding. Please use this instead of "..._e".
	typedef encoding_e encoding_t;
	// Type used to deal with file sizes of any weight (GBs are okay).
	typedef unsigned long filesize_t;



//-------------------------------------------------------------- Constants

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	// Returns true if the file exists, false otherwise.
	bool Exists(const char* filename); 

	// Returns true if trying to read charsToRead characters from the file fails.
	bool IsEmpty(const char* filename, size_t charsToRead = 0);
	
	// Closes ifs and tries to open filename according to Encoding's result.
	// Applies default locale if encoding is unknown.
	// To verify success, try ifs.is_open().
	// If an encoding is given, applies the corresponding locale 
	// (UTF-8 or UTF-16LE)
	// The position of ifs is then after useless BOMs.
	void Open(std::ifstream& ifs, const char* filename, 
		encoding_t encoding = ENC_UNKNOWN);

	filesize_t Size(const char* filename);

	// Returns the encoding of the file as one of the strings declared above.
	// It executes "IsEmpty(filename, 3)" firstly and if the result is true,
	// returns ENC_UNKNOWN.
	// If no encoding is found, returns ENC_DEFAULT.
	encoding_t Encoding(const char* filename);

	// Displays the file encoding as a string.
	std::ostream& operator<< (std::ostream& os, const encoding_t& enc);

#ifdef _WIN32
	// Returns true if the file exists, false otherwise.
	bool Exists(const wchar_t* filename);

	// Returns file size, or -1 in case of failure.
	filesize_t Size(const wchar_t* filename);
#endif
} 
#endif // FILE_H
