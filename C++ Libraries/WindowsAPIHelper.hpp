//
// Created by mfran on 13/04/2020.
//

#if !defined(MYWORKS_TEST0_WINDOWSAPIHELPER_HPP) && defined(_WIN32)
#define MYWORKS_TEST0_WINDOWSAPIHELPER_HPP

#include <string>
#include "File.hpp"

/**
 * Displays a message box with the error explanation, then terminates the program.
 * @param functionName Name of the function that failed.
 */
void _WindowsShowErrorMessage(const char* functionName);

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

using ProcessHandle = void*;

/**
 * Assuming that the given process is terminated, returns the exit code.
 * @param processHandle The process handle.
 * @return The exit code of the process.
 */
int _WindowsGetExitCodeCommand(ProcessHandle& processHandle);

/**
 * Creates a process by using the given command.
 * @param commandString Command to send to CMD.exe.
 * @param processHandle The process handle, which will be modified.
 * @return True.
 */
bool _WindowsCreateCommand(const std::string& commandString, ProcessHandle& processHandle);

/**
 * Waits forever for the given process to end. Returns when it exited.
 * @param processHandle The process to watch.
 */
void _WindowsWaitForProcess(ProcessHandle& processHandle);

/**
 * Waits some time, then kills the process. Returns when done.
 * @param processHandle The process to kill.
 * @param duration Time to wait, in milliseconds.
 */
void _WindowsReturnLaterCommand(ProcessHandle& processHandle, unsigned int duration);

/**
 * Forces a process to return now. Returns when done.
 * @param processHandle The process to kill.
 */
void _WindowsReturnNowProcess(ProcessHandle& processHandle);

// ///////////////////////////////////////////////////////////////
// /////////////////// FILE HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

using FileHandle = void*;

struct _WindowsReadFileData : public File::ReadFileData {
    FileHandle fileHandle = nullptr;
    FileHandle mappingHandle = nullptr;
};

/**
 * Deletes a file.
 * @param filename Name of the file to delete.
 * @return True on success, false on failure.
 */
bool _WindowsDeleteFile(File::filename_t filename);

/**
 * Deletes a directory.
 * @param directoryName Name of the directory to delete.
 * @return True on success, false on failure.
 */
bool _WindowsDeleteDirectory(File::filename_t directoryName);

/**
 * Checks whether a given file exists.
 * @param filename Name of the file to check.
 * @return True on success, false on failure.
 */
bool _WindowsFileExists(File::filename_t filename);

/**
 * Checks whether a given directory exists.
 * @param directoryName Name of the directory to check.
 * @return True on success, false on failure.
 */
bool _WindowsDirectoryExists(File::filename_t directoryName);

/**
 * Returns the size of a file.
 * @param filename Name of the file.
 * @return The size, or zero if anything failed.
 */
File::file_size_t _WindowsGetFileSize(File::filename_t filename);

/**
 * Creates a directory.
 * @param directoryName Name of the new directory.
 * @return True on success, false on failure.
 */
bool _WindowsCreateDirectory(File::filename_t directoryName);

/**
 * Gets the name of the current working directory.
 * @return A C++ string containing the current working directory.
 */
File::str_filename_t _WindowsGetCurrentWorkingDirectory();

/**
 * Generates a list of contents of a given directory.
 * @param directoryName Name of the directory.
 * @param result Vector of files/directory names to fill.
 */
void _WindowsGetDirectoryContents(File::filename_t directoryName, std::vector<File::str_filename_t>& result);

/**
 * Opens the given file and returns a pointer to a ReadFileData structure.
 * @param filename Name of the file to open.
 * @return A new structure, or nullptr if anything failed.
 */
const _WindowsReadFileData* _WindowsOpenFile(File::filename_t filename);

/**
 * Releases the memory associated with the file opened there.
 * @param readFileData A Windows ReadFileData to close ; its memory will also be freed.
 */
void _WindowsCloseReadFileData(const _WindowsReadFileData* readFileData);

#endif //MYWORKS_TEST0_WINDOWSAPIHELPER_HPP
