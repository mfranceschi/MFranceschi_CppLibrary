//
// Created by mfran on 24/04/2020.
//

#include "WindowsAPIHelper.hpp"
#include <Windows.h>
#include "StringSafePlaceHolder.hpp"

#if Threads_FOUND
#   include <thread>
#endif

/*
 * Sources:
 * https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output
 * https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes
 * https://docs.microsoft.com/fr-fr/windows/win32/fileio/appending-one-file-to-another-file?redirectedfrom=MSDN
 */

// PRIVATE DECLARATIONS

/**
 * Generates a HANDLE which will be the command's output stream, according to the choice.
 * @param outputChoice User's choice.
 * @param outputFile Name of the file to write into, if relevant.
 * @return A new Handle, or an invalid handle if it is not required or if it failed.
 */
static HANDLE setOutputHandle(const OutputChoice& outputChoice, const File::SFilename_t& outputFile);

/**
 * Adds the inherit property to the given handle.
 * @param handle The HANDLE to which the property will be applied.
 * @param inherit If true then we turn on inheritance for this handle; else we remove it.
 */
static void makeHandleInheritable(HANDLE handle, bool inherit = true);

/// Use this handle as a sample for inheritable handles.
static SECURITY_ATTRIBUTES securityAttributesForInheritableHandles {
    sizeof(SECURITY_ATTRIBUTES),
    nullptr,
    true
};

/**
 * Starts a process according to the command call settings.
 * The functions returns immediately after the new process is started.
 * @param commandCall Settings for the start
 * @param processHandle The process Handle will be written here
 * @param handlesToClose List of handles to close when the process is over. It will be modified during the call.
 *                       It includes at least one element: the process handle.
 * @return True on success, false on failure
 */
static bool Windows_StartProcess(
        const CommandCall& commandCall,
        Windows_ProcessHandle& processHandle,
        std::vector<Windows_ProcessHandle>& handlesToClose,
        Windows_ProcessHandle processStdHandles[3]);

// PRIVATE DEFINITIONS

static HANDLE setOutputHandle(const OutputChoice& outputChoice, const File::SFilename_t& outputFile) {
    const TCHAR* lpFileName;
    DWORD dwDesiredAccess = FILE_GENERIC_WRITE;
    DWORD dwShareMode = 0;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = &securityAttributesForInheritableHandles;
    DWORD dwCreationDisposition = OPEN_ALWAYS;
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    HANDLE hTemplateFile = nullptr;

    switch(outputChoice) {
        case OutputChoice::KEEP:
            return INVALID_HANDLE_VALUE;

        case OutputChoice::KILL:
            lpFileName = TEXT("nul");
            dwFlagsAndAttributes |= FILE_ATTRIBUTE_DEVICE;
            break;

        case OutputChoice::EXPORT:
            lpFileName = outputFile.c_str();
            break;

        case OutputChoice::EXPORT_APPEND:
            lpFileName = outputFile.c_str();
            dwDesiredAccess |= FILE_APPEND_DATA;
            break;

        case OutputChoice::RETRIEVE:
            // TODO same but we must be able to retrieve the data (pipe?)
            break;
    }

    HANDLE fileHandle = CreateFile(
            lpFileName,
            dwDesiredAccess,
            dwShareMode,
            lpSecurityAttributes,
            dwCreationDisposition,
            dwFlagsAndAttributes,
            hTemplateFile);

    if (outputChoice == OutputChoice::EXPORT_APPEND) {
        SetFilePointer(fileHandle, 0, nullptr, FILE_END);
    }
    return fileHandle;
}

static void makeHandleInheritable(HANDLE handle, bool inherit) {
    SetHandleInformation(handle, HANDLE_FLAG_INHERIT, inherit ? HANDLE_FLAG_INHERIT : 0);
}

static bool Windows_StartProcess(
        const CommandCall& commandCall,
        Windows_ProcessHandle& processHandle,
        std::vector<Windows_ProcessHandle>& handlesToClose,
        Windows_ProcessHandle processStdHandles[3]) {
    // Define all parameters required by the CreateProcess function.
    auto lpApplicationName = commandCall.executable.c_str();
    TCHAR* lpCommandLine;
    {
        // Fill the "Command line" string
        File::OSStream_t osStream;
        for (const auto& arg : commandCall.arguments) {
            osStream << arg << " ";
        }
        auto streamOutput = osStream.str();

        const TCHAR* commandStringPointer = streamOutput.c_str();
        lpCommandLine = new TCHAR[streamOutput.size() + 1];
        auto copyResult = StringCchCopy(lpCommandLine, streamOutput.size() + 1, commandStringPointer);

        if ( FAILED(copyResult) ) {
            if (copyResult == STRSAFE_E_INVALID_PARAMETER) {
                Windows_ShowErrorMessage("StringCchCopy <invalid param>");
            } else {
                Windows_ShowErrorMessage("StringCchCopy <insufficient buffer>");
            }
        }
    }
    LPSECURITY_ATTRIBUTES lpProcessAttributes = nullptr; // No specific security attribute
    LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr; // No specific security attribute
    bool bInheritHandles = false; // Handles are inherited
    DWORD dwCreationFlags = 0; // Creation flags
    LPVOID lpEnvironment = nullptr; // We use the parent's environment
    const TCHAR* lpCurrentDirectory = nullptr; // We use the parent's current working directory
    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processInformation;
    LPSTARTUPINFO lpStartupInfo = &startupinfo;
    LPPROCESS_INFORMATION lpProcessInformation = &processInformation;
    {
        ZeroMemory(&startupinfo, sizeof(startupinfo));
        startupinfo.cb = sizeof(startupinfo);
        ZeroMemory(&processInformation, sizeof(processInformation));
        // By default, we have nothing in these structs
    }

    // Do some specific stuff.
    if (
            (commandCall.outputChoice != OutputChoice::KEEP) ||
            (commandCall.errorsChoice != ErrorChoice::KEEP) ||
            (commandCall.inputChoice != InputChoice::NONE)) {
        bInheritHandles = true;
        startupinfo.dwFlags |= STARTF_USESTDHANDLES;
        startupinfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        startupinfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        startupinfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    }
    {
        HANDLE lastHandle;

        lastHandle = setOutputHandle(commandCall.outputChoice, commandCall.outputFile);
        if (lastHandle != INVALID_HANDLE_VALUE) {
            startupinfo.hStdOutput = lastHandle;
            handlesToClose.push_back(lastHandle);
            makeHandleInheritable(lastHandle, true);
            processStdHandles[0] = lastHandle;
        }

        lastHandle = setOutputHandle(commandCall.errorsChoice, commandCall.errorFile);
        if (lastHandle != INVALID_HANDLE_VALUE) {
            startupinfo.hStdError = lastHandle;
            handlesToClose.push_back(lastHandle);
            makeHandleInheritable(lastHandle, true);
            processStdHandles[1] = lastHandle;
        }

        // TODO handle input
    }

    bool createProcessResult = CreateProcess(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation
    );
    if (!createProcessResult) {
        Windows_ShowErrorMessage("CreateProcess");
        // TODO remove later
    }

    processHandle = processInformation.hProcess;
    CloseHandle(processInformation.hThread);

    handlesToClose.push_back(processHandle);
    return createProcessResult;
}

// PUBLIC DEFINITIONS

void Windows_Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    Windows_ProcessHandle processHandle;
    std::vector<Windows_ProcessHandle> handlesToClose; // List of handles to close when the child process is over.
    HANDLE processStdHandles[3] = {INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
    File::SFilename_t outputsTempFile, errorsTempFile;

    Windows_StartProcess(commandCall, processHandle, handlesToClose, processStdHandles);
    switch (commandCall.returnChoice) {
        case ReturnChoice::WHEN_DONE: {
            Windows_WaitForProcess(processHandle);
            commandReturn.returnCode = Windows_GetExitCodeCommand(processHandle);
            File::ReadToString(outputsTempFile.c_str(), commandReturn.outputText);
            File::ReadToString(errorsTempFile.c_str(), commandReturn.errorText);
        }
            break;
        case ReturnChoice::IMMEDIATELY:
        case ReturnChoice::FUNCTION: {
            switch (commandCall.interruptChoice) {
#if Threads_FOUND
                case InterruptChoice::NEVER: {
                    std::thread processThread([&]() {
                        Windows_WaitForProcess(processHandle);
                        if (commandCall.returnChoice == ReturnChoice::FUNCTION) {
                            CommandReturn newCommandReturn;
                            newCommandReturn.returnCode = Windows_GetExitCodeCommand(processHandle);
                            File::ReadToString(outputsTempFile.c_str(), commandReturn.outputText);
                            File::ReadToString(errorsTempFile.c_str(), commandReturn.errorText);
                            commandCall.returnFunction(newCommandReturn);
                        }
                    });
                    processThread.detach();
                    break;
                }
                case InterruptChoice::AFTER_TIME: {
                    std::thread processThread([&]() {
                        Windows_ReturnLaterCommand(processHandle, commandCall.executionDuration);
                        if (commandCall.returnChoice == ReturnChoice::FUNCTION) {
                            CommandReturn newCommandReturn;
                            newCommandReturn.returnCode = Windows_GetExitCodeCommand(processHandle);
                            File::ReadToString(outputsTempFile.c_str(), commandReturn.outputText);
                            File::ReadToString(errorsTempFile.c_str(), commandReturn.errorText);
                            commandCall.returnFunction(newCommandReturn);
                        }
                    });
                    processThread.detach();
                    break;
                }
#else
                    case InterruptChoice::NEVER:
                case InterruptChoice::AFTER_TIME: {
                    // TODO
                    break;
                }
#endif
                case InterruptChoice::ON_DEMAND: {
                    // TODO
                    break;
                }
            }
            break;
        }
    }
}
