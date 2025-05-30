//
// Created by MartinF on 30/05/2025.
//

#include "command_test_commons.hpp"

TEST_F(CommandTestBase, LengthOfFirstArg) {
    // Features verified:
    // - The arguments passing and maybe parsing is behaving as expected.
    commandCall.executable = TEXT(LengthOfFirstArg_Executable);

    // Empty arg vector -> no argv[1] -> returns 0.
    commandCall.arguments = {};
    callCommand(0);

    // Simple string of length 5 -> returns 5.
    commandCall.arguments = {TEXT("abcde")};
    callCommand(5);

    // Empty/blank string as arg -> should be ignored.
    commandCall.arguments = {TEXT(" ")};
    // callCommand(0); // TODO: to keep?

    // String of length 1 but surrounded by escaped double quotes ->
    // the parser receives a string of length 3, but it unwraps the string ->
    // we should end up with only 1.
    commandCall.arguments = {TEXT("\"a\"")};
    callCommand(1);

    // Same as above but it's an edge case where we force a blank string to be passed as arg.
    // The parser unwraps the string -> should return 1;
    commandCall.arguments = {TEXT("\" \"")};
    callCommand(1);
}
