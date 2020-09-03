//
// Created by mfran on 23/04/2020.
//

#include "tests_datas.hpp"

class Commands : public ::testing::Test {
protected:
    CommandCall commandCall;
    CommandReturn commandReturn;
    void cc();
};

void Commands::cc() {
    Command(commandCall, commandReturn);
}

TEST_F(Commands, ReturnNbArgs_Nothing) {
    commandCall.executable = ReturnNbArgs_Executable;
    cc();
    EXPECT_EQ(1, commandReturn.returnCode);
}

TEST_F(Commands, HelloWorld_Simple) {
    commandCall.executable = HelloWorld_Executable;
    cc();
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_TRUE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, HelloWorld_KilledOutput) {
    commandCall.executable = HelloWorld_Executable;
    commandCall.outputChoice = OutputChoice::KILL;
    cc();
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_TRUE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, HelloWorld_RetrieveAllOutputs) {
    commandCall.executable = HelloWorld_Executable;
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    //commandCall.errorChoice = ErrorChoice::RETRIEVE;
    cc();
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_FALSE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
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

    EXPECT_EQ(3, commandReturn.returnCode);

    EXPECT_FALSE(commandReturn.outputText.empty());
    EXPECT_STREQ((commandCall.inputString + LINE_END).c_str(), commandReturn.outputText.c_str());

    EXPECT_FALSE(commandReturn.errorText.empty());
    File::OSStream_t oss;
    for (std::size_t i = 0; i < commandCall.arguments.size(); i++) {
        oss << (i + 1) << ": " << commandCall.arguments[i] << LINE_END;
    }
    EXPECT_STREQ(oss.str().c_str(), commandReturn.errorText.c_str());
}

TEST_F(Commands, LengthOfFirstArg) {
    commandCall.executable = LengthOfFirstArg_Executable;

    commandCall.arguments = {};
    cc();
    ASSERT_EQ(commandReturn.returnCode, 0) << "Bad config";

    commandCall.arguments = {"\"a\""};
    cc();
    EXPECT_EQ(commandReturn.returnCode, 1);

    commandCall.arguments = {"abcde"};
    cc();
    EXPECT_EQ(commandReturn.returnCode, 5);

    commandCall.arguments = {"\" \""};
    cc();
    EXPECT_EQ(commandReturn.returnCode, 1);
}

TEST_F(Commands, LengthOfInput) {
        //GTEST_SKIP();
    commandCall.executable = LengthOfInput_Executable;

    commandCall.inputChoice = InputChoice::STRING;
    commandCall.inputString = "abcde\n";
    cc();
    EXPECT_EQ(commandReturn.returnCode, 5);
}
