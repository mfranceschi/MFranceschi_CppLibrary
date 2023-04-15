//
// Created by mfran on 23/04/2020.
//

#include "MF/Command.hpp"

#include "tests_data.hpp"

using namespace ::MF::Command;

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
    commandCall.executable = ReturnNbArgs_Executable;
    callCommand(1);
}

TEST_F(CommandTestBase, HelloWorld_OutputIgnored) {
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}

TEST_F(CommandTestBase, HelloWorld_OutputToConsole) {
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputToConsole();
    callCommand();
}

TEST_F(CommandTestBase, HelloWorld_OutputToFile) {
    const Filename_t filename = Filename_t(test_info_->name()) + ".txt";
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputToFile(filename);
    callCommand();

    std::ifstream ifstream(filename, std::ios_base::in);
    std::string line;
    std::getline(ifstream, line);
    ifstream.close();
    EXPECT_EQ(line, std::string("Hello, World!"));

    MF::Filesystem::deleteFile(filename);
}

TEST_F(CommandTestBase, HelloWorldToStderr_OutputToFile) {
    const Filename_t filename = Filename_t(test_info_->name()) + ".txt";
    commandCall.executable = HelloWorldToStderr_Executable;
    commandCall.stdErrChoice = makeOutputToFile(filename);
    callCommand();

    std::ifstream ifstream(filename, std::ios_base::in);
    std::string line;
    std::getline(ifstream, line);
    ifstream.close();
    EXPECT_EQ(line, std::string("Hello, World!"));

    MF::Filesystem::deleteFile(filename);
}

TEST_F(CommandTestBase, HelloWorld_OutputToStringStream) {
    std::stringstream stream;
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputToStringStream(stream);
    callCommand();

    std::string line;
    std::getline(stream, line);
    EXPECT_EQ(line, std::string("Hello, World!"));
}

/*
TEST_F(Commands, HelloWorld_RetrieveAllOutputs) {
    commandCall.executable = HelloWorld_Executable;
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    // commandCall.errorChoice = ErrorChoice::RETRIEVE;
    cc();
    EXPECT_EQ(0, exitCode);
    EXPECT_FALSE(commandOver.outputText.empty());
    EXPECT_TRUE(commandOver.errorText.empty());
}

TEST_F(Commands, OneForEachStream) {
    GTEST_SKIP();
    commandCall.executable = OneForEachStream_Executable;
    commandCall.arguments = {"one", "two"};
    commandCall.inputChoice = InputChoice::STRING;
    commandCall.inputString = "input0";
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    commandCall.errorChoice = OutputChoice::RETRIEVE;

    cc();

    EXPECT_EQ(3, exitCode);

    EXPECT_FALSE(commandOver.outputText.empty());
    EXPECT_STREQ((commandCall.inputString + LINE_END).c_str(), commandOver.outputText.c_str());

    EXPECT_FALSE(commandOver.errorText.empty());
    std::ostringstream oss;
    for (std::size_t i = 0; i < commandCall.arguments.size(); i++) {
        oss << (i + 1) << ": " << commandCall.arguments[i] << LINE_END;
    }
    EXPECT_STREQ(oss.str().c_str(), commandOver.errorText.c_str());
}*/

TEST_F(CommandTestBase, LengthOfFirstArg) {
    commandCall.executable = LengthOfFirstArg_Executable;

    commandCall.arguments = {};
    callCommand();

    commandCall.arguments = {"\"a\""};
    callCommand(1);

    commandCall.arguments = {"abcde"};
    callCommand(5);

    commandCall.arguments = {"\" \""};
    callCommand(1);
}

TEST_F(CommandTestBase, LengthOfInput_FromString) {
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromString(
        "abcde\n"
        "fghij\n");
    callCommand(5);
}

TEST_F(CommandTestBase, LengthOfInput_FromStringStream) {
    std::stringstream stream;
    stream << "abcde\n"
           << "fghij\n";

    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromStringStream(stream);
    callCommand(5);
}

TEST_F(CommandTestBase, LengthOfInput_InputStreamClosed) {
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputEmpty();
    callCommand();
}

TEST_F(CommandTestBase, LengthOfInput_FromFile) {
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromFile(LOREM_IPSUM_TWO_LINES_FILE);
    callCommand(95);
}

TEST_F(CommandTestBase, GenerateOutput_Ignored) {
    commandCall.executable = GenerateOutput_Executable;
    commandCall.arguments = {std::to_string(2), "out"};
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}
