/* Martin Franceschi - MyWorks package - File module.
 * 
 * This module gives various informations about a given file.
 * You can check if it exists, check its size and encoding.
 * You can also open it by applying automatically a locale
 * (UTF-8 and UTF-16LE only).
 * You are in charge of providing the right C-string filename (wide or not).
 */

//---------- Interface of module <File> (file File.h) 
#if ! defined ( FILE_H )
#define FILE_H

//--------------------------------------------------------------- Includes
#include <fstream>
#include <initializer_list>
#include <string>
#include <vector>

#include "Toolbox.hpp"

namespace File
{
	// Represents a file encoding.
	// It is an implementation detail that may change. Please use "..._t" instead.
	enum class encoding_e {
		ENC_UTF16LE, // Normal UTF-16LE
		ENC_UTF8, // Normal UTF-8
		ENC_DEFAULT, // If no encoding is false, we assume the default locale.
		ENC_ERROR // A problem occured while looking for the encoding.
	};

	// Represents a file encoding.
	typedef encoding_e encoding_t;
	
	// Type used to deal with file sizes of any weight (GBs are okay).
	typedef unsigned long filesize_t;

	// Type used for file names.
#if defined _WIN32 && defined UNICODE
	typedef const wchar_t* filename_t; // typedef for const wchar_t *
	typedef std::wstring sfilename_t; // typedef for std::wstring	
#else
	typedef const char* filename_t; // typedef for const char *
	typedef std::string sfilename_t; // typedef for std::string
#endif
	

//-------------------------------------------------------------- Constants

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	// Deletes the file or the directory.
	// If the "fileonly" parameter is true and the target is a directory then it fails.
	// Returns true on success, false otherwise.
	bool Delete(filename_t filename, bool fileonly = true);

	// Returns the encoding of the file as one of the strings declared above.
	// It executes "IsEmpty(filename, 3)" firstly and if the result is true,
	// returns ENC_ERROR.
	// If no encoding is found, returns ENC_DEFAULT.
	encoding_t Encoding(filename_t filename);

	// Returns true if the file exists, false otherwise.
	bool Exists(filename_t filename); 

	// Returns true if the given path points to a directory, false otherwise.
	bool IsDir(filename_t filename);

	// Returns true if trying to read "charsToRead" characters from the file fails.
	// If "charsToRead" <= 0 then it is ignored and the function only checks if the file can be opened.
	bool IsEmpty(filename_t filename, int charsToRead = 3);
	
	// Closes "ifs" and tries to open the file "filename".
	// If an encoding is given, applies the corresponding locale 
	// (UTF-8 or UTF-16LE) ; else applies default locale.
	// The ifs.tellg() is after BOMs.
	// Returns true on success, false otherwise.
	bool Open(std::ifstream& ifs, filename_t filename, 
		encoding_t encoding = encoding_t::ENC_ERROR);

	// Returns the entire file as a const char*.
	// Please note that it does not end with a '\0'.
	// If file size is zero or if it fails, returns nullptr.
	// For closing always use Read_Close.
	// Thread-safe.
	const char* Read(filename_t filename);

	// Closes a file opened using the Read function.
	// Returns whether a file was closed successfully.
	// Thread-safe.
	bool Read_Close(const char* content);

	// Returns the file size in bytes, or 0.
	filesize_t Size(filename_t filename);

	// Displays the file encoding as a string.
	std::ostream& operator<< (std::ostream& os, const encoding_t& enc);

	// Creates a folder.
	// On Windows, new folder's access rights are the ones of the parent's dir.
	// Otherwise it is 777.
	bool CreateFolder(filename_t filename);

	// Returns Current Working Directory.
	// Array created using malloc --> please use free (not delete[]).
	filename_t GetCWD();

	// Returns an unordered list of files that matches the given patterns.
	// Returns empty result is "patterns" is empty or no pattern was found.
	std::vector<File::sfilename_t> MatchPattern(const std::vector<File::sfilename_t>& patterns);
	inline std::vector<File::sfilename_t> MatchPattern(const std::initializer_list<File::sfilename_t>& patterns);
} 

//------------------------------------------------------ Other definitions

inline std::vector<File::sfilename_t> File::MatchPattern(const std::initializer_list<File::sfilename_t>& patterns)
{
	std::vector<File::sfilename_t> patterns_vec(patterns.begin(), patterns.end());
	return File::MatchPattern(patterns_vec);
}
#endif // FILE_H
