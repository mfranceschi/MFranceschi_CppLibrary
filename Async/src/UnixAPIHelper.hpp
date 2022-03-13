//
// Created by mfran on 19/04/2020.
//

#if !defined( MFRANCESCHI_CPPLIBRARIES_UNIXAPIHELPER_HPP) && !defined(_WIN32)
#define MFRANCESCHI_CPPLIBRARIES_UNIXAPIHELPER_HPP

#include "MF/Filesystem.hpp"

namespace MF {

    using namespace Filesystem;

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////
// /////////////////// FILE HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

    struct Unix_ReadFileData : public ReadFileData {
        int fd = -1;
    };

/**
 * Deletes a file.
 * @param filename Name of the file to delete.
 * @return True on success, false on failure.
 */
    bool Unix_DeleteFile(Filename_t filename);

/**
 * Deletes a file or a directory.
 * @param name Name of the directory to delete.
 * @return True on success, false on failure.
 */
    bool Unix_DeleteFileOrDirectory(Filename_t name);

/**
 * Checks whether a given file exists.
 * @param filename Name of the file to check.
 * @return True on success, false on failure.
 */
    bool Unix_FileExists(Filename_t filename);

/**
 * Checks whether a given directory exists.
 * @param directoryName Name of the directory to check.
 * @return True on success, false on failure.
 */
    bool Unix_DirectoryExists(Filename_t directoryName);

/**
 * Returns the size of a file.
 * @param filename Name of the file.
 * @return The size, or zero if anything failed.
 */
    Filesize_t Unix_GetFileSize(Filename_t filename);

/**
 * Creates a directory.
 * @param directoryName Name of the new directory.
 * @return True on success, false on failure.
 */
    bool Unix_CreateDirectory(Filename_t directoryName);

/**
 * Gets the name of the current working directory.
 * @return A C++ string containing the current working directory.
 */
    SFilename_t Unix_GetCurrentWorkingDirectory();

/**
 * Generates a list of contents of a given directory.
 * @param directoryName Name of the directory.
 * @param result Vector of files/directory names to fill.
 */
    void Unix_GetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result);

/**
 * Opens the given file and returns a pointer to a ReadFileData structure.
 * @param filename Name of the file to open.
 * @return A new structure, or nullptr if anything failed.
 */
    const Unix_ReadFileData *Unix_OpenFile(Filename_t filename);

/**
 * Releases the memory associated with the file opened there.
 * @param readFileData An Unix ReadFileData to close ; its memory will also be freed.
 */
    void Unix_CloseReadFileData(const Unix_ReadFileData *readFileData);

/// For polymorphic reasons; actually this is a dummy function.
    void Unix_CloseReadFileData(const ReadFileData *readFileData);

/**
 * Fills the given buffer with the few first chars of the given file.
 * @param filename File to read. No ending Null-character.
 * @param buffer Buffer to fill.
 * @param bufferSize Length of the buffer.
 * @return Number of bytes written, -1 on failure.
 */
    int Unix_ReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize);
}

#endif //MFRANCESCHI_CPPLIBRARIES_UNIXAPIHELPER_HPP
