//
// Created by mfran on 23/04/2020.
//

#include "command_test_commons.hpp"

TEST_F(CommandTestBase, GenerateOutput_Ignored) {
    // TODO
    commandCall.executable = TEXT(GenerateOutput_Executable);
    commandCall.arguments = {toStr(2), TEXT("out")};
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}
