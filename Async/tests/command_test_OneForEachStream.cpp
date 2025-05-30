//
// Created by MartinF on 30/05/2025.
//

#if 0 // TODO: Enable this test when the executable is available.

#    include "command_test_commons.hpp"

TEST_F(CommandTestBase, DISABLED_OneForEachStream) {
    // This is the Super Smash Bros. of the tests.
    // - We give it arguments. They are basically repeated on StdErr.
    // - We give it some StdIn. It's repeated on StdOut.
    // - We check at the exit code = the number of args, including the executable name.
    commandCall.executable = TEXT(OneForEachStream_Executable);
    commandCall.arguments = {TEXT("one1"), TEXT("two222")};

    const std::basic_string<TCHAR> inputArg = TEXT("input0123");
    commandCall.stdInChoice = makeInputFromString(inputArg);

    std::basic_stringstream<TCHAR> outStream;
    commandCall.stdOutChoice = makeOutputToIOStream(outStream);

    std::basic_stringstream<TCHAR> errStream;
    commandCall.stdErrChoice = makeOutputToIOStream(errStream);

    callCommand(3);

    auto outString = outStream.str();
    EXPECT_FALSE(outString.empty());
    EXPECT_EQ(inputArg + lineEnd, outString);

    auto errString = errStream.str();
    EXPECT_FALSE(errString.empty());
    std::basic_ostringstream<TCHAR> expectedErr;
    expectedErr << TEXT("1: ") << commandCall.arguments[0] << lineEnd;
    expectedErr << TEXT("2: ") << commandCall.arguments[1] << lineEnd;
    EXPECT_EQ(expectedErr.str(), errString);
}
#endif
