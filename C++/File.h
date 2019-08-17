/* Martin Franceschi - MyWorks package - File module.
 * 
 * This module gives some basic informations about a given file.
 * You can check if it exists, check its size and encoding.
 * You can also open it by applying automatically a locale
 * (UTF-8 and UTF-16LE only).
 * All method are made so that it is the fastest possible way to achieve
 * these results.
 * You are in charge of providing the right C-string filename (wide or not).
 * 
 * In particular, the functions "Exists" and "Size" are as fast as possible.
 * 
 */

//---------- Interface of module <File> (file File.h) 
#if ! defined ( FILE_H )
#define FILE_H

//--------------------------------------------------------------- Includes
#include <fstream>
#include <sstream>
#include <string>
#include "Toolbox.h"

namespace File
{
	// Represents a file encoding.
	// It is an implementation detail that may change. Please use "..._t" instead.
	enum encoding_e {
		ENC_UTF16LE, // Normal UTF-16LE
		ENC_UTF8, // Normal UTF-8
		ENC_DEFAULT, // If no encoding is false, we assume the default locale.
		ENC_UNKNOWN // A problem occured while looking for the encoding.
	};

	// Represents a file encoding.
	typedef encoding_e encoding_t;
	
	// Type used to deal with file sizes of any weight (GBs are okay).
	typedef unsigned long filesize_t;

	// Type used for file names.
#if defined _WIN32 && defined UNICODE
	typedef const wchar_t* filename_t; // typedef for const wchar_t *
#else
	typedef const char* filename_t; // typedef for const char *
#endif

//-------------------------------------------------------------- Constants

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	// Deletes the file or the directory.
	// If the "fileonly" parameter is true and the target is a directory then it fails.
	// Returns true on success, false otherwise.
	bool Delete(filename_t filename, bool fileonly = false);

	// Returns the encoding of the file as one of the strings declared above.
	// It executes "IsEmpty(filename, 3)" firstly and if the result is true,
	// returns ENC_UNKNOWN.
	// If no encoding is found, returns ENC_DEFAULT.
	encoding_t Encoding(filename_t filename);

	// Returns true if the file exists, false otherwise.
	bool Exists(filename_t filename); 

	// Returns true if the given path points to a directory, false otherwise.
	bool IsDir(filename_t filename);

	// Returns true if trying to read charsToRead characters from the file fails.
	bool IsEmpty(filename_t filename, size_t charsToRead = 0);
	
	// Closes ifs and tries to open filename according to Encoding's result.
	// Applies default locale if encoding is unknown.
	// To verify success, try ifs.is_open().
	// If an encoding is given, applies the corresponding locale 
	// (UTF-8 or UTF-16LE)
	// The position of ifs is then after useless BOMs.
	void Open(std::ifstream& ifs, filename_t filename, 
		encoding_t encoding = ENC_UNKNOWN);

	// Returns the entire file as a const char*.
	// Please note that it does not end with a '\0'.
	// If file size is zero or if it fails, returns nullptr.
	// For closing always use Read_Close.
	const char* Read(filename_t filename);

	// Closes a file opened using the Read function.
	// Returns whether a file was closed successfully.
	bool Read_Close(const char* content);

	// Returns the file size in bytes, or 0.
	filesize_t Size(filename_t filename);

	// Displays the file encoding as a string.
	std::ostream& operator<< (std::ostream& os, const encoding_t& enc);
} 
#endif // FILE_H
