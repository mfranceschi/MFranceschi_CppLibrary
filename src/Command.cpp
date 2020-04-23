//
// Created by mfran on 09/04/2020.
//

#if defined(_WIN32)
#  include "WindowsAPIHelper.hpp"
#endif

#if Threads_FOUND
#   include <thread>
#endif
#include <sstream>
#include "File.hpp"
#include "Command.hpp"

// PRIVATE DECLARATIONS

/**
 * Given a command call, generates the string to pass to the command interpreter.
 * @param commandCall The command call settings
 * @return A string to use.
 */
static File::SFilename_t _PrepareCommandString(const CommandCall& commandCall);

/**
 * Copies every byte from the file to the string. Maximum-optimized.
 * @param filename URL to the file to open.
 * @param toFill String to modify.
 */
static void _FillStringWithFileContents(const File::SFilename_t& filename, File::SFilename_t& toFill);

// PRIVATE DEFINITIONS

File::SFilename_t _PrepareCommandString(const CommandCall& commandCall) {
    File::OSStream_t oss;

    oss << "\"" << commandCall.executable << "\" ";

    for (const auto& argument : commandCall.arguments) {
        oss << argument << " ";
    }

    // TODO handle in the good order input output error

    switch (commandCall.inputChoice) {
        case InputChoice::NONE:
            break;

        case InputChoice::STRING:
        case InputChoice::FUNCTION: {
            File::SFilename_t input;

            if (commandCall.inputChoice == InputChoice::STRING) {
                input = commandCall.inputString;
            } else {
                input = commandCall.inputFunction();
            }

            oss.seekp(0, std::ios_base::beg);
            oss << "echo \"" << input << "\" | ";
            oss.seekp(0, std::ios_base::end);
            break;
        }
        case InputChoice::FROM_FILE: {
            decltype(oss)::pos_type currentPosition = oss.tellp();
            oss << " < " << commandCall.inputFile;
            oss.seekp(currentPosition);
            break;
        }
    }

    return oss.str();
}

void _FillStringWithFileContents(const File::SFilename_t& filename, std::string& toFill) {
    auto fileContents = File::Read(filename.c_str());
    if (fileContents) {
        const char *contents = fileContents->contents;
        std::size_t length = File::Size(filename.c_str());
        toFill.assign(contents, length);
        File::Read_Close(fileContents);
    }
}


// PUBLIC DEFINITIONS

void Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    File::SFilename_t commandString = _PrepareCommandString(commandCall);
    File::SFilename_t outputsTempFile;
    File::SFilename_t errorsTempFile;

#if defined(_WIN32)
    ProcessHandle processHandle;
    _WindowsCreateCommand(commandCall, processHandle);
    switch (commandCall.returnChoice) {
        case ReturnChoice::WHEN_DONE: {
            _WindowsWaitForProcess(processHandle);
            commandReturn.returnCode = _WindowsGetExitCodeCommand(processHandle);
            _FillStringWithFileContents(outputsTempFile, commandReturn.outputText);
            _FillStringWithFileContents(errorsTempFile, commandReturn.errorText);
        }
            break;
        case ReturnChoice::IMMEDIATELY:
        case ReturnChoice::FUNCTION: {
            switch (commandCall.interruptChoice) {
#if Threads_FOUND
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

#endif
}
