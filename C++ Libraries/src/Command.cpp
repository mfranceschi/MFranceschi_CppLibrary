//
// Created by mfran on 09/04/2020.
//

#if defined(_WIN32)
#  include "WindowsAPIHelper.hpp"
#endif

#include <thread> // TODO use MinGW-STD-Threads
#include <sstream>
#include "File.hpp"
#include "Command.hpp"

// PRIVATE DECLARATIONS

/**
 * Given a command call, generates the string to pass to the command interpreter.
 * @param commandCall The command call settings
 * @return A string to use.
 */
static std::string _PrepareCommandString(const CommandCall& commandCall);

/**
 * Copies every byte from the file to the string. Maximum-optimized.
 * @param filename URL to the file to open.
 * @param toFill String to modify.
 */
static void _FillStringWithFileContents(const std::string& filename, std::string& toFill);

// PRIVATE DEFINITIONS

std::string _PrepareCommandString(const CommandCall& commandCall) {
    std::ostringstream oss;

    oss << "\"" << commandCall.executable << "\" ";

    for (const std::string& argument : commandCall.arguments) {
        oss << argument << " ";
    }

    // TODO handle in the good order input output error

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

    return oss.str();
}

void _FillStringWithFileContents(const std::string& filename, std::string& toFill) {
    auto fileContents = File::Read(filename.c_str());
    const char* contents = fileContents->contents;
    std::size_t length = File::Size(filename.c_str());
    toFill.assign(contents, length);
    File::Read_Close(fileContents);
}


// PUBLIC DEFINITIONS

void Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    std::string commandString = _PrepareCommandString(commandCall);
    std::string outputsTempFile;
    std::string errorsTempFile;

#if defined(_WIN32)
    ProcessHandle processHandle;
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
