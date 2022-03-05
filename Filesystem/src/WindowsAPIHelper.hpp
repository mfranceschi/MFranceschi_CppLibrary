//
// Created by mfran on 13/04/2020.
//

#if !defined(MYWORKS_TEST0_WINDOWSAPIHELPER_HPP) && defined(_WIN32)
#define MYWORKS_TEST0_WINDOWSAPIHELPER_HPP

#include <string>
#include "MF/Filesystem.hpp"
#include "MF/FileOpen.hpp"

namespace MF {
/**
 * Displays a message box with the error explanation, then terminates the program.
 * @param functionName Name of the function that failed.
 */
    void Windows_ShowErrorMessage(const char *functionName);

/**
 * Generates a newly-allocated (using the "new" operator) wchar_t array from the given Multi-Bytes string.
 * @param utf8String Source.
 * @return Newly-allocated string, created by copying the contents with the correct syscall.
 */
    const wchar_t *Windows_ConvertString(const char *utf8String);

/// Grants or removes the inheritance property to the given handle.
    void Windows_MakeHandleInheritable(void *handle, bool inherit = true);

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

    using Windows_ProcessHandle = void *;

/**
 * Assuming that the given process is terminated, returns the exit code.
 * @param processHandle The process handle.
 * @return The exit code of the process.
 */
    int Windows_GetExitCodeCommand(Windows_ProcessHandle &processHandle);

/**
 * Waits forever for the given process to end. Returns when it exited.
 * @param processHandle The process to watch.
 */
    void Windows_WaitForProcess(Windows_ProcessHandle &processHandle);

/**
 * Waits some time, then kills the process. Returns when done.
 * @param processHandle The process to kill.
 * @param duration Time to wait, in milliseconds.
 */
    void Windows_ReturnLaterCommand(Windows_ProcessHandle &processHandle, unsigned int duration);

/**
 * Forces a process to return now. Returns when done.
 * @param processHandle The process to kill.
 */
    void Windows_ReturnNowProcess(Windows_ProcessHandle &processHandle);

// ///////////////////////////////////////////////////////////////
// /////////////////// FILE HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

    using Windows_FileHandle = void *;

    struct Windows_ReadFileData : public File::ReadFileData {
        Windows_FileHandle fileHandle = nullptr;
        Windows_FileHandle mappingHandle = nullptr;
    };

/**
 * Deletes a file.
 * @param filename Name of the file to delete.
 * @return True on success, false on failure.
 */
    bool Windows_DeleteFile(File::Filename_t filename);

/**
 * Deletes a directory.
 * @param directoryName Name of the directory to delete.
 * @return True on success, false on failure.
 */
    bool Windows_DeleteDirectory(File::Filename_t directoryName);

/**
 * Checks whether a given file exists.
 * @param filename Name of the file to check.
 * @return True on success, false on failure.
 */
    bool Windows_FileExists(File::Filename_t filename);

/**
 * Checks whether a given directory exists.
 * @param directoryName Name of the directory to check.
 * @return True on success, false on failure.
 */
    bool Windows_DirectoryExists(File::Filename_t directoryName);

/**
 * Returns the size of a file.
 * @param filename Name of the file.
 * @return The size, or zero if anything failed.
 */
    File::Filesize_t Windows_GetFileSize(File::Filename_t filename);

/**
 * Creates a directory.
 * @param directoryName Name of the new directory.
 * @return True on success, false on failure.
 */
    bool Windows_CreateDirectory(File::Filename_t directoryName);

/**
 * Gets the name of the current working directory.
 * @return A C++ string containing the current working directory.
 */
    File::SFilename_t Windows_GetCurrentWorkingDirectory();

/**
 * Generates a list of contents of a given directory.
 * @param directoryName Name of the directory.
 * @param result Vector of files/directory names to fill.
 */
    void Windows_GetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t> &result);

/**
 * Opens the given file and returns a pointer to a ReadFileData structure.
 * @param filename Name of the file to open.
 * @return A new structure, or nullptr if anything failed.
 */
    const Windows_ReadFileData *Windows_OpenFile(File::Filename_t filename);

/**
 * Releases the memory associated with the file opened there.
 * @param readFileData A Windows ReadFileData to close ; its memory will also be freed.
 */
    void Windows_CloseReadFileData(const Windows_ReadFileData *readFileData);

/// For polymorphic reasons; actually this is a dummy function.
    void Windows_CloseReadFileData(const File::ReadFileData *readFileData);

/**
 * Fills the given buffer with the few first chars of the given file.
 * @param filename File to read. No ending Null-character.
 * @param buffer Buffer to fill.
 * @param bufferSize Length of the buffer.
 * @return Number of bytes written, -1 on failure.
 */
    int Windows_ReadFileToBuffer(File::Filename_t filename, char *buffer, File::Filesize_t bufferSize);
}
#endif //MYWORKS_TEST0_WINDOWSAPIHELPER_HPP
