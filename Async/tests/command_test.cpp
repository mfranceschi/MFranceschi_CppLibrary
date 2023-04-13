//
// Created by mfran on 23/04/2020.
//

#include "MF/Command.hpp"

#include "tests_data.hpp"

using namespace ::MF::Command;

class Commands : public ::testing::Test {
   protected:
    CommandCall commandCall;

    int exitCode;

    void cc() {
        exitCode = runCommandAndWait(commandCall).exitCode;
    }
};

TEST_F(Commands, ReturnNbArgs_Nothing) {
    commandCall.executable = ReturnNbArgs_Executable;
    cc();
    EXPECT_EQ(1, exitCode);
}

TEST_F(Commands, HelloWorld_KilledOutput) {
    commandCall.executable = HelloWorld_Executable;
    commandCall.stdOutChoice = makeOutputIgnored();
    commandCall.stdErrChoice = makeOutputIgnored();
    cc();
    EXPECT_EQ(0, exitCode);
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

TEST_F(Commands, LengthOfFirstArg) {
    commandCall.executable = LengthOfFirstArg_Executable;

    commandCall.arguments = {};
    cc();
    ASSERT_EQ(exitCode, 0) << "Bad config";

    commandCall.arguments = {"\"a\""};
    cc();
    EXPECT_EQ(exitCode, 1);

    commandCall.arguments = {"abcde"};
    cc();
    EXPECT_EQ(exitCode, 5);

    commandCall.arguments = {"\" \""};
    cc();
    EXPECT_EQ(exitCode, 1);
}

TEST_F(Commands, LengthOfInput) {
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromString("abcde\n");
    cc();
    EXPECT_EQ(exitCode, 5);
}

TEST_F(Commands, LengthOfInput_InputStreamClosed) {
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputEmpty();
    cc();
    EXPECT_EQ(exitCode, 0);
}

TEST_F(Commands, LengthOfInput_FromFile) {
    commandCall.executable = LengthOfInput_Executable;
    commandCall.stdInChoice = makeInputFromFile(LOREM_IPSUM_TWO_LINES_FILE);
    cc();
    EXPECT_EQ(exitCode, 95);
}

TEST_F(Commands, GenerateOutput_FromFile) {
    commandCall.executable = GenerateOutput_Executable;
    commandCall.arguments = {std::to_string(1e5), "out"};
    commandCall.stdOutChoice = makeOutputIgnored();
    cc();
    EXPECT_EQ(exitCode, 95);
}
