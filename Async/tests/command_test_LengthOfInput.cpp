//
// Created by MartinF on 30/05/2025.
//

#include "command_test_commons.hpp"

#if 0 // TODO: Re-enable this test when the LengthOfInput executable is available.

TEST_F(CommandTestBase, LengthOfInput_FromString) {
    // Features verified:
    // - String input works as intended
    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputFromString(TEXT("abcde\nfghij\n"));
    callCommand(5 * sizeof(TCHAR));
}

TEST_F(CommandTestBase, LengthOfInput_FromStringStream) {
    // Features verified:
    // - StringStream input works as intended
    std::basic_stringstream<TCHAR> stream;
    stream << "abcde\n"
           << "fghij\n";

    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputFromIOStream(stream);
    callCommand(5 * sizeof(TCHAR));
}

TEST_F(CommandTestBase, LengthOfInput_InputStreamClosed) {
    // Features verified:
    // - Empty input works as intended AND does not cause the child process to be stuck.
    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputEmpty();
    callCommand();
}

TEST_F(CommandTestBase, LengthOfInput_FromFile) {
    // Features verified:
    // - File input works as intended
    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputFromFile(TEXT(LOREM_IPSUM_TWO_LINES_FILE));
    callCommand(174);
}
#endif
