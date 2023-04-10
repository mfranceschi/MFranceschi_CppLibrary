//
// Created by mfran on 13/04/2020.
//

#if MF_WINDOWS

#    include "WindowsAPIHelper.hpp"

#    include "MF/LightWindows.hpp"
#    include "MF/Windows.hpp"

// ///////////////////////////////////////////////////////////////
// //////////////// COMMAND HANDLING API /////////////////////////
// ///////////////////////////////////////////////////////////////

int Windows_GetExitCodeCommand(Windows_ProcessHandle &processHandle) {
    DWORD exitCode;
    if (GetExitCodeProcess(processHandle, &exitCode)) {
        return static_cast<int>(exitCode);
    } else {
        MF::Windows::ShowErrorMessage("GetExitCodeProcess");
        return 3; // Will never be called because of "ExitProcess".
    }
}

void Windows_WaitForProcess(Windows_ProcessHandle &processHandle) {
    WaitForSingleObject(processHandle, INFINITE);
}

void Windows_ReturnLaterCommand(HANDLE &processHandle, unsigned int duration) {
    WaitForSingleObject(processHandle, duration);
    Windows_ReturnNowProcess(processHandle);
}

void Windows_ReturnNowProcess(Windows_ProcessHandle &processHandle) {
    TerminateProcess(processHandle, -1);
    Windows_WaitForProcess(processHandle);
}

#endif
