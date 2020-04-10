//
// Created by mfran on 09/04/2020.
//

#include <cstdlib>
#include <sstream>

#if defined(WIN32)
#   define _CRT_SECURE_NO_WARNINGS 1
#   include <Windows.h>
#endif

#include "Command.hpp"

// Code taken from https://stackoverflow.com/a/28876046/11996851
template <typename T>
struct is_chrono_duration
{
    static constexpr bool value = false;
};

template <typename Rep, typename Period>
struct is_chrono_duration<std::chrono::duration<Rep, Period>>
{
    static constexpr bool value = true;
};

template <typename Duration_t>
static void _PrepareCommandString(const CommandCall<Duration_t>& commandCall, std::string commandString) {
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
    return createProcessResult;
}

void _WindowsWaitForProcess(HANDLE& processHandle) {
    WaitForSingleObject(processHandle, INFINITE);
}

void _WindowsReturnNowCommand(HANDLE& processHandle) {
    TerminateProcess(processHandle, -1);
    _WindowsWaitForProcess(processHandle);
}

template <typename Duration_t>
void _WindowsReturnLaterCommand(HANDLE& processHandle, const Duration_t& duration) {
    DWORD durationDword = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    WaitForSingleObject(processHandle, durationDword);
    TerminateProcess(processHandle, -1);
    _WindowsWaitForProcess(processHandle);
}

int _WindowsGetExitCodeCommand(HANDLE& processHandle) {
    DWORD exitCode;
    GetExitCodeProcess(processHandle, &exitCode);
    return static_cast<int>(exitCode);
}

template <typename Duration_t>
void Command(const CommandCall<Duration_t>& commandCall, CommandReturn& commandReturn) {
    static_assert(is_chrono_duration<Duration_t>::value, "The duration requires to be of type std::chrono::duration");
    std::string commandString;
    _PrepareCommandString(commandCall, commandString);

#if defined(WIN32)
    HANDLE processHandle;
    _WindowsCreateCommand(commandString, processHandle);
    switch (commandCall.returnChoice) {
        case ReturnChoice::WHEN_DONE:
            _WindowsReturnNowCommand(processHandle);
            commandReturn.returnCode = _WindowsGetExitCodeCommand(processHandle);
            break;
        case ReturnChoice::IMMEDIATELY:

    }

#endif


    commandReturn.returnCode = returnCode;
}
