//
// Created by mfran on 13/04/2020.
//

#if !defined(MYWORKS_TEST0_WINDOWSAPIHELPER_HPP) && MF_WINDOWS
#    define MYWORKS_TEST0_WINDOWSAPIHELPER_HPP

#    include <string>

#    include "MF/Command.hpp"
#    include "MF/Filesystem.hpp"

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

#endif // MYWORKS_TEST0_WINDOWSAPIHELPER_HPP
