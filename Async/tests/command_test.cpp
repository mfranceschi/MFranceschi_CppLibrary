//
// Created by mfran on 23/04/2020.
//

#include "MF/Command.hpp"

#include "tests_data.hpp"

using namespace ::MF::Command;

static const std::string HELLO_WORLD = "Hello, World!";

class CommandTestBase : public ::testing::Test {
   protected:
    CommandCall commandCall;

    int exitCode = -1;

    void callCommand(int expectedExitCode = 0) {
        exitCode = runCommandAndWait(commandCall).exitCode;
        EXPECT_EQ(expectedExitCode, exitCode);
    }
};

TEST_F(CommandTestBase, ReturnNbArgs_Nothing) {
    // Features verified:
    // - We can call Command
    // - Empty vector of arguments = no additional argument, so that the "argc" equals one.
    // - We can get the return code
    // - Default options do not fail
    commandCall.executable = ReturnNbArgs_Executable;
    callCommand(1);
}

TEST_F(CommandTestBase, ReturnNbArgs_OneArg) {
    // Features verified:
    // - We can specify at least 1 argument
    commandCall.executable = ReturnNbArgs_Executable;
    commandCall.arguments = {"arg-one"};
    callCommand(2);
}

TEST_F(CommandTestBase, LengthOfFirstArg) {
    // Features verified:
    // - The arguments passing and maybe parsing is behaving as expected.
    commandCall.executable = LengthOfFirstArg_Executable;

    // Empty arg vector -> no argv[1] -> returns 0.
    commandCall.arguments = {};
    callCommand(0);

    // Simple string of length 5 -> returns 5.
    commandCall.arguments = {"abcde"};
    callCommand(5);

    // Empty/blank string as arg -> should be ignored.
    commandCall.arguments = {" "};
    callCommand(0);

    // String of length 1 but surrounded by escaped double quotes ->
    // the parser receives a string of length 3, but it unwraps the string ->
    // we should end up with only 1.
    commandCall.arguments = {"\"a\""};
    callCommand(1);

    // Same as above but it's an edge case where we force a blank string to be passed as arg.
    // The parser unwraps the string -> should return 1;
    commandCall.arguments = {"\" \""};
    callCommand(1);
}

TEST_F(CommandTestBase, HelloWorld_OutputIgnored) {
    // Features verified:
    // - Ignored output does not break
    // NOTE: this does NOT check that nothing was written to the console.
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}

TEST_F(CommandTestBase, HelloWorld_OutputToConsole) {
    // Features verified:
    // - Console output does not break
    // NOTE: this does NOT check that something was written to the console.
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputToConsole();
    callCommand();
}

TEST_F(CommandTestBase, HelloWorld_OutputToFile) {
    // Features verified:
    // - File output works as intended
    const Filename_t filename = Filename_t(test_info_->name()) + ".txt";
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputToFile(filename);
    callCommand();

    std::ifstream fileStream(filename, std::ios_base::in);
    std::string line;
    std::getline(fileStream, line);
    fileStream.close();
    EXPECT_EQ(line, HELLO_WORLD);

    MF::Filesystem::deleteFile(filename);
}

TEST_F(CommandTestBase, HelloWorld_OutputToStringStream) {
    // Features verified:
    // - StringStream output works as intended
    std::stringstream stream;
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputToStringStream(stream);
    callCommand();

    std::string line;
    std::getline(stream, line);
    EXPECT_EQ(line, HELLO_WORLD);
}

TEST_F(CommandTestBase, HelloWorld_RetrieveAllOutputs) {
    // Features verified:
    // - Stream redirection is distinct between StdOut and StdErr
    // - No output = nothing is written in the corresponding StringStream
    commandCall.executable = HelloWorldToStderr_Executable;
    std::stringstream outStream;
    outStream << "1";
    std::stringstream errStream;
    errStream << "2";
    commandCall.stdOutChoice = makeOutputToStringStream(outStream);
    commandCall.stdErrChoice = makeOutputToStringStream(errStream);

    callCommand();

    EXPECT_EQ(outStream.str(), "1");
    EXPECT_EQ(errStream.str(), "2" + HELLO_WORLD);
}

TEST_F(CommandTestBase, LengthOfInput_FromString) {
    // Features verified:
    // - String input works as intended
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromString(
        "abcde\n"
        "fghij\n");
    callCommand(5);
}

TEST_F(CommandTestBase, LengthOfInput_FromStringStream) {
    // Features verified:
    // - StringStream input works as intended
    std::stringstream stream;
    stream << "abcde\n"
           << "fghij\n";

    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromStringStream(stream);
    callCommand(5);
}

TEST_F(CommandTestBase, LengthOfInput_InputStreamClosed) {
    // Features verified:
    // - Empty input works as intended AND does not cause the child process to be stuck.
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputEmpty();
    callCommand();
}

TEST_F(CommandTestBase, LengthOfInput_FromFile) {
    // Features verified:
    // - File input works as intended
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromFile(LOREM_IPSUM_TWO_LINES_FILE);
    callCommand(95);
}

TEST_F(CommandTestBase, OneForEachStream) {
    // This is the Super Smash Bros. of the tests.
    // - We give it arguments. They are basically repeated on StdErr.
    // - We give it some StdIn. It's repeated on StdOut.
    // - We check at the exit code = the number of args, including the executable name.
    commandCall.executable = OneForEachStream_Executable;
    commandCall.arguments = {"one1", "two222"};
    const std::string inputArg = "input0123";
    commandCall.stdInChoice = makeInputFromString(inputArg);
    std::stringstream outStream;
    std::stringstream errStream;
    commandCall.stdOutChoice = makeOutputToStringStream(outStream);
    commandCall.stdErrChoice = makeOutputToStringStream(errStream);

    callCommand(3);

    EXPECT_EQ(3, exitCode);

    auto outString = outStream.str();
    EXPECT_FALSE(outString.empty());
    EXPECT_EQ(inputArg + MF::Filesystem::LINE_END, outString);

    auto errString = errStream.str();
    EXPECT_FALSE(errString.empty());
    std::ostringstream oss;
    oss << "1: " << commandCall.arguments[0] << MF::Filesystem::LINE_END;
    oss << "2: " << commandCall.arguments[1] << MF::Filesystem::LINE_END;
    EXPECT_STREQ(oss.str().c_str(), errString.c_str());
}

TEST_F(CommandTestBase, GenerateOutput_Ignored) {
    // TODO
    commandCall.executable = GenerateOutput_Executable;
    commandCall.arguments = {std::to_string(2), "out"};
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}
