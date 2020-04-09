//
// Created by mfran on 09/04/2020.
//

#include <cstdlib>
#include <sstream>

#include "Command.hpp"
void Command(const CommandCall& commandCall, CommandReturn& commandReturn) {
    std::ostringstream oss;

    oss << commandCall.executable << " ";
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
            std::ostringstream::pos_type currentPosition = oss.tellp();
            oss << " < " << commandCall.inputFile;
            oss.seekp(currentPosition);
            break;
        }
    }

    int returnCode = system(oss.str().c_str());

    commandReturn.returnCode = returnCode;
}
