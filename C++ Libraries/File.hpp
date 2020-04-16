/* Martin Franceschi - MyWorks package - File module.
 * 
 * This module gives various information about a given file.
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
		ENC_ERROR // A problem occurred while looking for the encoding.
	};

	// Represents a file encoding.
	typedef encoding_e encoding_t;
	
	// Type used to deal with file sizes of any weight (GBs are okay).
	typedef unsigned long file_size_t;

	// Type used for file names.
#if defined _WIN32 && defined UNICODE
	typedef const wchar_t* filename_t; // typedef for const wchar_t *
	typedef std::wstring str_filename_t; // typedef for std::wstring
#else
	typedef const char* filename_t; // typedef for const char *
	typedef std::string str_filename_t; // typedef for std::string
#endif

    // Data structure used to store information about files opened with Open.
    struct ReadFileData {
        const char* contents = nullptr; // Holds the file data.
        file_size_t size = 0ul; // Size of the file.
        virtual ~ReadFileData() = default; // For polymorphic reasons.
    };

//-------------------------------------------------------------- Constants
#if defined _WIN32
#   define FILE_SEPARATOR R"slash(\)slash"
#   if defined UNICODE
#       define MAKE_FILE_NAME L""
#   else
#       define MAKE_FILE_NAME ""
#   endif
#else
#   define FILE_SEPARATOR R"slash(/)slash"
#   define MAKE_FILE_NAME ""
#endif

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	/**
	 * This function removes the given file or directory.
	 * @param filename Name of the file or directory to remove. Often, directories require to be empty.
	 * @param fileOnly If true, the removal is done if and only if the "filename" points to a file.
	 * @return True if something was removed successfully.
	 */
	bool Delete(filename_t filename, bool fileOnly = true);

	/**
	 * Determines the encoding of the file, accordingly to the first two or three bytes in the file.
	 * Special results: ENC_ERROR if reading the first three bytes failed; ENC_DEFAULT if no supported encoding could be determined.
	 * @param filename Path to the file to test.
	 * @return An "encoding_t" value.
	 */
	encoding_t Encoding(filename_t filename);

	/// Returns true if "filename" points to an existing file, false otherwise.
	bool Exists(filename_t filename); 

	/// Returns true if "filename" points to an existing directory, false otherwise.
	bool IsDir(filename_t filename);

	/**
	 * Attempts to read a few bytes in the file, and simply say if it was a success or not.
	 * @param filename Name of the file to read.
	 * @param charsToRead Number of bytes to read. If <= 0, we simply return "Exist(filename)".
	 * @return True if it was a success, false if we could not read exactly "charsToRead" bytes.
	 */
	bool CanReadFile(filename_t filename, int charsToRead = 3);
	
	/**
	 * Extraordinary function.
	 *
	 * 1. It closes "ifs".
	 * 2. If unknown, it calls "Encoding(filename)" to determine the encoding.
	 * 3. If the coding could be processed, we call "ifs.open(filename)" with a specific policy (locale, starting offset, etc.).
	 * In all cases, the function returns.
	 * @param ifs The stream to open with the given file and opening policy. It is closed before doing anything.
	 * @param filename The path to the file to open.
	 * @param encoding (optional) If you already know the encoding, you can set this parameter;
	 *                            otherwise it will be determined inside the function.
	 * @return False if and only if trying to determine the encoding failed --> file cannot be processed correctly.
	 */
	bool Open(std::ifstream& ifs, filename_t filename, 
		encoding_t encoding = encoding_t::ENC_ERROR);

	/**
	 * Stores the entire contents of file "filename" in a read-only C-string.
	 * Also, that C-string does not end with '\0'.
	 * It is advised to use an "InCharArrayStream".
	 * To clean up memory, please call "Read_Close" with the structure returned from there.
	 * The structure must remain a pointer (in reality, it is an instance of a subclass of ReadFileData).
	 * The purpose of this function is to offer the fastest way to read an entire file.
	 * @param filename Name of the file to open.
	 * @return "nullptr" if anything failed or the file is empty, or a new structure.
	 */
	const ReadFileData* Read(filename_t filename);

	/// Please use this simple tool to clean up any memory associated with something returned by "Read".
	void Read_Close(const ReadFileData* content);

	/// Returns the size of the file pointed by "filename" in bytes, or 0 if anything failed.
	file_size_t Size(filename_t filename);

	/// Simple helper function, for use during debugging.
	std::ostream& operator<< (std::ostream& os, const encoding_t& enc);

	/**
	 * Creates an empty directory. No particular setting (such as permissions) is applied.
	 * @param filename Name of the folder to create.
	 * @return True if the folder actually got created.
	 */
	bool CreateFolder(filename_t filename);

	/// Returns the "Current Working Directory" as a folder path. It ends with a FILE_SEPARATOR.
	str_filename_t GetCWD();

	/**
	 * Generates the complete list of files and directories that are direct children of the given folder.
	 * Names are returned relative to the "folder". Directories have an ending PATH_SEPARATOR.
	 * > FilesInDirectory("foldername/") -> ("file.txt", "image.png", "subfolder/")
	 * @param folder Name or path to the folder. It must end with a PATH_SEPARATOR character.
	 * @return List of files and directories names, or empty vector if anything failed.
	 */
	std::vector<File::str_filename_t> FilesInDirectory(filename_t folder);
} 

//------------------------------------------------------ Other definitions

#endif // FILE_H
