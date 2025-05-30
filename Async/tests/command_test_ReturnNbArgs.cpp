//
// Created by MartinF on 30/05/2025.
//

#include "command_test_commons.hpp"

TEST_F(CommandTestBase, ReturnNbArgs_Nothing) {
    // Features verified:
    // - We can call Command
    // - Empty vector of arguments = no additional argument, so that the "argc" equals one.
    // - We can get the return code
    // - Default options do not fail
    commandCall.executable = TEXT(ReturnNbArgs_Executable);
    callCommand(1);
}

TEST_F(CommandTestBase, ReturnNbArgs_OneArg) {
    // Features verified:
    // - We can specify at least 1 argument
    commandCall.executable = TEXT(ReturnNbArgs_Executable);
    commandCall.arguments = {TEXT("arg-one")};
    callCommand(2);
}
