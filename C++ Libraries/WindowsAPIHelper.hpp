//
// Created by mfran on 13/04/2020.
//

#ifndef MYWORKS_TEST0_WINDOWSAPIHELPER_HPP
#define MYWORKS_TEST0_WINDOWSAPIHELPER_HPP

#if defined(_WIN32)

#include <string>
#include "File.hpp"

using ProcessHandle = void*;

/**
 * Displays a message box with the error explanation, then exits.
 * @param functionName Name of the function that failed.
 */
void _WindowsShowErrorMessage(const char* functionName);

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

bool _WindowsDeleteFile(File::filename_t filename);
bool _WindowsDeleteDirectory(File::filename_t directoryName);
bool _WindowsFileExists(File::filename_t filename);
bool _WindowsDirectoryExists(File::filename_t directoryName);
File::file_size_t _WindowsGetFileSize(File::filename_t filename);
bool _WindowsCreateDirectory(File::filename_t directoryName);
File::str_filename_t _WindowsGetCurrentWorkingDirectory();
void _WindowsGetDirectoryContents(File::filename_t directoryName, std::vector<File::str_filename_t>);

#endif // defined _WIN32

#endif //MYWORKS_TEST0_WINDOWSAPIHELPER_HPP
