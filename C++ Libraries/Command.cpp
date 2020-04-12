//
// Created by mfran on 09/04/2020.
//

#include <cstdlib>
#include <thread>

#include <sstream>
#if defined(WIN32)
#   include <Windows.h>
#include <strsafe.h>
#endif

#include "File.hpp"
#include "Command.hpp"

static void _PrepareCommandString(const CommandCall& commandCall, std::string commandString) {
    std::ostringstream oss;

    if (commandCall.executable.find(' ') != std::string::npos) {
        oss << "\"" << commandCall.executable << "\" ";
    } else {
        oss << commandCall.executable << " ";
    }
    for (const std::string& argument : commandCall.arguments) {
        oss << argument << " ";
    }

    switch (commandCall.inputChoice) {
        case InputChoice::NONE:
            break;

        case InputChoice::STRING:
        case InputChoice::FUNCTION: {
            std::string input;

            if (commandCall.inputChoice == InputChoice::STRING) {
                input = commandCall.inputData;
            } else {
                input = commandCall.inputFunction();
            }

            oss.seekp(0, std::ios_base::beg);
            oss << "echo \"" << input << "\" | ";
            oss.seekp(0, std::ios_base::end);
            break;
        }
        case InputChoice::FROM_FILE: {
            std::ostringstream::pos_type currentPosition = oss.tellp();
            oss << " < " << commandCall.inputData;
            oss.seekp(currentPosition);
            break;
        }
    }

    commandString = oss.str();
}

int _WindowsGetExitCodeCommand(HANDLE& processHandle);
bool _WindowsCreateCommand(const std::string& commandString, HANDLE& processHandle) {
    const char* commandStringPointer = commandString.c_str();
    char* newCommandString = new char[commandString.size() + 1];
    std::strcpy(newCommandString, commandStringPointer);

    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processInformation;
    SecureZeroMemory(&startupinfo, sizeof(startupinfo));
    SecureZeroMemory(&processInformation, sizeof(processInformation));
    startupinfo.cb = sizeof(startupinfo);
    bool createProcessResult = CreateProcess(
            nullptr,    // No module name (use command line)
            newCommandString,
            nullptr,// Process handle not inheritable
            nullptr,// Thread handle not inheritable
            false,// Set handle inheritance to FALSE
            0,// No creation flags
            nullptr,// Use parent's environment block
            nullptr,// Use parent's starting directory
            &startupinfo,
            &processInformation
            );
    processHandle = processInformation.hProcess;
    CloseHandle(processInformation.hThread);
    _WindowsGetExitCodeCommand(processHandle);
    return createProcessResult;
}

void _WindowsWaitForProcess(HANDLE& processHandle) {
    WaitForSingleObject(processHandle, INFINITE);
}

void _WindowsReturnNowCommand(HANDLE& processHandle) {
    TerminateProcess(processHandle, -1);
    _WindowsWaitForProcess(processHandle);
}

void _WindowsReturnLaterCommand(HANDLE& processHandle, unsigned int duration) {
    WaitForSingleObject(processHandle, duration);
    TerminateProcess(processHandle, -1);
    _WindowsWaitForProcess(processHandle);
}

int _WindowsGetExitCodeCommand(HANDLE& processHandle) {
    DWORD exitCode;
    if (GetExitCodeProcess(processHandle, &exitCode)) {
        return static_cast<int>(exitCode);
    } else {
        // Retrieve the system error message for the last-error code

        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError();

        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

        // Display the error message and exit the process

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                          (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)"GetExitCodeProcess") + 40) * sizeof(TCHAR));
        StringCchPrintf((LPTSTR)lpDisplayBuf,
                        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                        TEXT("%s failed with error %d: %s"),
                        "GetExitCodeProcess", dw, lpMsgBuf);
        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
        ExitProcess(dw);
    }
}

void _FillStringWithFileContents(const std::string& filename, std::string toFill) {
    const char* contents = File::Read(filename.c_str());
    std::size_t length = File::Size(filename.c_str());
    toFill.reserve(length);
    toFill.assign(contents, length);
    File::Read_Close(contents);
}

void Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    std::string commandString;
    _PrepareCommandString(commandCall, commandString);
    std::string outputsTempFile;
    std::string errorsTempFile;

#if defined(_WIN32)
    HANDLE processHandle;
    _WindowsCreateCommand(commandString, processHandle);
    switch (commandCall.returnChoice) {
        case ReturnChoice::WHEN_DONE:
            _WindowsWaitForProcess(processHandle);
            commandReturn.returnCode = _WindowsGetExitCodeCommand(processHandle);
            _FillStringWithFileContents(outputsTempFile, commandReturn.outputText);
            _FillStringWithFileContents(errorsTempFile, commandReturn.errorText);
            break;
        case ReturnChoice::IMMEDIATELY:
        case ReturnChoice::FUNCTION:
        {
            switch (commandCall.interruptChoice) {
                case InterruptChoice::NEVER: {
                    std::thread processThread([&]() {
                        _WindowsWaitForProcess(processHandle);
                        if (commandCall.returnChoice == ReturnChoice::FUNCTION) {
                            CommandReturn newCommandReturn;
                            newCommandReturn.returnCode = _WindowsGetExitCodeCommand(processHandle);
                            _FillStringWithFileContents(outputsTempFile, newCommandReturn.outputText);
                            _FillStringWithFileContents(errorsTempFile, newCommandReturn.errorText);
                            commandCall.returnFunction(newCommandReturn);
                        }
                    });
                    processThread.detach();
                    break;
                }
                case InterruptChoice::AFTER_TIME: {
                    std::thread processThread([&]() {
                        _WindowsReturnLaterCommand(processHandle, commandCall.executionDuration);
                        if (commandCall.returnChoice == ReturnChoice::FUNCTION) {
                            CommandReturn newCommandReturn;
                            newCommandReturn.returnCode = _WindowsGetExitCodeCommand(processHandle);
                            _FillStringWithFileContents(outputsTempFile, newCommandReturn.outputText);
                            _FillStringWithFileContents(errorsTempFile, newCommandReturn.errorText);
                            commandCall.returnFunction(newCommandReturn);
                        }
                    });
                    processThread.detach();
                    break;
                }
                case InterruptChoice::ON_DEMAND: {
                    // TODO
                    break;
                }
            }
            break;
        }
    }

#endif
}
