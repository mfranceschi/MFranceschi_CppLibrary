//
// Created by mfran on 09/04/2020.
//

#ifndef MYWORKS_TEST0_COMMAND_HPP
#define MYWORKS_TEST0_COMMAND_HPP

#include <string>
#include <vector>
#include <functional>
#include "File.hpp"

enum class OutputChoice {
    KEEP, // Let it on the console
    KILL, // Silent and ignore
    EXPORT, // Write outputs into the file "outputFile"
    EXPORT_APPEND, // Appends outputs into the file "outputFile"
    RETRIEVE // Get outputs as string in the return structure
};

enum class ErrorChoice {
    KEEP, // Let errors on the console
    KILL, // Silent and ignore
    EXPORT, // Write errors into the file "outputFile"
    EXPORT_APPEND, // Appends errors into the file "outputFile"
    RETRIEVE, // Get errors as string in the return structure
    LIKE_OUTPUTS // Use the same setting as for the outputs
};

enum class InputChoice {
    NONE, // Nothing
    STRING, // Give the string "inputString"
    FUNCTION, // Generate the input by calling the function "inputFunction"
    FROM_FILE // Gives the file named "inputString" as input
};

enum class ReturnChoice {
    WHEN_DONE, // Return the function right after it finishes (no new thread created)
    IMMEDIATELY, // The function returns now and the command runs independently
    FUNCTION // Same as IMMEDIATELY, but also calls the given function when done
};

enum class InterruptChoice {
    NEVER, // No restriction for the execution
    AFTER_TIME, // Interrupts the execution after the given time
    ON_DEMAND // Interrupts the execution when some function is called (returned in CommandReturn)
};

struct CommandReturn {
    int returnCode = 0; // Return value of the command
    std::string outputText; // [?] Complete string of the outputs
    std::string errorText; // [?] Complete string of the errors
    std::function<void()> callToTerminate; // [?] Call this to (try to) force the command to terminate
};

struct CommandCall {
    std::string executable; // Name or path to the executable
    std::vector<std::string> arguments; // List of arguments to the executable, they will be concatenated with " ".
    std::string outputFile; // [?] File in which to write outputs
    OutputChoice outputChoice = OutputChoice::KEEP; // Choice for outputs
    std::string errorFile; // [?] File in which to write errors
    ErrorChoice errorsChoice = ErrorChoice::KEEP; // Choice for errors
    std::function<const char*()> inputFunction; // [?] Function to retrieve inputs
    std::string inputData; // [?] String or file as input
    InputChoice inputChoice = InputChoice::NONE; // Choice for inputs
    std::function<void(CommandReturn&)> returnFunction;
    ReturnChoice returnChoice = ReturnChoice::WHEN_DONE;
    unsigned int executionDuration = -1; // In milliseconds
    InterruptChoice interruptChoice = InterruptChoice::NEVER;
};

void Command(const CommandCall& call, CommandReturn&);
// TODO implement
// - Normal call
// - CMD specific call
// - PowerShell specific call
// - Bash specific call
// - Shell specific call
// - Le shell bizarre specific call
// - Script Bash
// - Script PowerShell
// - Script Shell
// - Script Bash
// - Get list of available stuff (bitfield: Bash, PowerShell, etc.)
// - Common structure of parameters: quiet option, etc.

#endif //MYWORKS_TEST0_COMMAND_HPP
