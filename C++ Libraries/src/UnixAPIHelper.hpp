//
// Created by mfran on 19/04/2020.
//

#if !defined( MFRANCESCHI_CPPLIBRARIES_UNIXAPIHELPER_HPP) && !defined(_WIN32)
#define MFRANCESCHI_CPPLIBRARIES_UNIXAPIHELPER_HPP

#include "File.hpp"

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////
// /////////////////// FILE HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

struct _UnixReadFileData : public File::ReadFileData {
    int fd = -1;
};

/**
 * Deletes a file.
 * @param filename Name of the file to delete.
 * @return True on success, false on failure.
 */
bool _UnixDeleteFile(File::Filename_t filename);

/**
 * Deletes a file or a directory.
 * @param name Name of the directory to delete.
 * @return True on success, false on failure.
 */
bool _UnixDeleteFileOrDirectory(File::Filename_t name);

/**
 * Checks whether a given file exists.
 * @param filename Name of the file to check.
 * @return True on success, false on failure.
 */
bool _UnixFileExists(File::Filename_t filename);

/**
 * Checks whether a given directory exists.
 * @param directoryName Name of the directory to check.
 * @return True on success, false on failure.
 */
bool _UnixDirectoryExists(File::Filename_t directoryName);

/**
 * Returns the size of a file.
 * @param filename Name of the file.
 * @return The size, or zero if anything failed.
 */
File::Filesize_t _UnixGetFileSize(File::Filename_t filename);

/**
 * Creates a directory.
 * @param directoryName Name of the new directory.
 * @return True on success, false on failure.
 */
bool _UnixCreateDirectory(File::Filename_t directoryName);

/**
 * Gets the name of the current working directory.
 * @return A C++ string containing the current working directory.
 */
File::SFilename_t _UnixGetCurrentWorkingDirectory();

/**
 * Generates a list of contents of a given directory.
 * @param directoryName Name of the directory.
 * @param result Vector of files/directory names to fill.
 */
void _UnixGetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t>& result);

/**
 * Opens the given file and returns a pointer to a ReadFileData structure.
 * @param filename Name of the file to open.
 * @return A new structure, or nullptr if anything failed.
 */
const _UnixReadFileData* _UnixOpenFile(File::Filename_t filename);

/**
 * Releases the memory associated with the file opened there.
 * @param readFileData An Unix ReadFileData to close ; its memory will also be freed.
 */
void _UnixCloseReadFileData(const _UnixReadFileData* readFileData);

/// For polymorphic reasons; actually this is a dummy function.
void _UnixCloseReadFileData(const File::ReadFileData* readFileData);

/**
 * Fills the given buffer with the few first chars of the given file.
 * @param filename File to read. No ending Null-character.
 * @param buffer Buffer to fill.
 * @param bufferSize Length of the buffer.
 * @return Number of bytes written, -1 on failure.
 */
int _UnixReadFileToBuffer(File::Filename_t filename, char* buffer, File::Filesize_t bufferSize);

#endif //MFRANCESCHI_CPPLIBRARIES_UNIXAPIHELPER_HPP
