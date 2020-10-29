//
// Created by mfran on 09/04/2020.
//

#if Threads_FOUND
#   include <thread>
#endif
#include "MF/File.hpp"
#include "MF/Command.hpp"
#include "CommandHelper.hpp"

// PUBLIC DEFINITIONS

void Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    CommandRunner runner;
    runner.executable = &commandCall.executable;
    runner.arguments = &commandCall.arguments;

    switch (commandCall.outputChoice) {
        case OutputChoice::KEEP:
            break;
        case OutputChoice::KILL:
            runner.setOutput(new ProcessOutputStream_Kill());
            break;
        case OutputChoice::EXPORT:
            runner.setOutput(new ProcessOutputStream_Export(false, commandCall.outputFile));
            break;
        case OutputChoice::EXPORT_APPEND:
            runner.setOutput(new ProcessOutputStream_Export(true, commandCall.outputFile));
            break;
        case OutputChoice::RETRIEVE:
            runner.setOutput(new ProcessOutputStream_Retrieve());
            break;
    }

    switch (commandCall.errorChoice) {
        case ErrorChoice::KEEP:
            break;
        case ErrorChoice::KILL:
            runner.setError(new ProcessOutputStream_Kill());
            break;
        case ErrorChoice::EXPORT:
            runner.setError(new ProcessOutputStream_Export(false, commandCall.outputFile));
            break;
        case ErrorChoice::EXPORT_APPEND:
            runner.setError(new ProcessOutputStream_Export(true, commandCall.outputFile));
            break;
        case ErrorChoice::RETRIEVE:
            runner.setError(new ProcessOutputStream_Retrieve());
            break;
    }

    switch (commandCall.inputChoice) {
        case InputChoice::NONE:
            break;
        case InputChoice::STRING:
            runner.setInput(new ProcessInputStream_String(commandCall.inputString));
            break;
        case InputChoice::FROM_FILE:
            runner.setInput(new ProcessInputStream_FromFile(commandCall.inputFile));
            break;
    }

    runner.start();
    // TODO handle program stopping option
    runner.stop();

    if (commandCall.outputChoice == OutputChoice::RETRIEVE) {
        commandReturn.outputText = runner.getOutput();
    }
    if (commandCall.errorChoice == ErrorChoice::RETRIEVE) {
        commandReturn.errorText = runner.getError();
    }
    commandReturn.returnCode = runner.getStatusCode();
}
