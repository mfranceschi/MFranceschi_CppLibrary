//
// Created by mfran on 23/04/2020.
//

#include "tests_datas.hpp"

class Commands : public ::testing::Test {
protected:
    CommandCall commandCall;
    CommandReturn commandReturn;
    void cc();
    void pwd_cmd();
};

#if defined(_WIN32)
    void Commands::pwd_cmd() {
        commandCall.executable = "echo";
        commandCall.arguments = {"%cd%"};
    }
#else
    void Commands::pwd_cmd() {
        commandCall.executable = "pwd";
    }
#endif

void Commands::cc() {
    Command(commandCall, commandReturn);
}

TEST_F(Commands, ReturnNbArgs_Nothing) {
    commandCall.executable = ReturnNbArgs_Executable;
    cc();
    EXPECT_EQ(1, commandReturn.returnCode);
}

TEST_F(Commands, PWD_Simple) {
    pwd_cmd();
    cc();
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_TRUE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, PWD_KilledOutput) {
    pwd_cmd();
    commandCall.outputChoice = OutputChoice::KILL;
    cc();
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_TRUE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, PWD_RetrieveAllOutputs) {
    GTEST_SKIP();
    pwd_cmd();
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    commandCall.errorChoice = ErrorChoice::RETRIEVE;
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
    EXPECT_STREQ("input0\n", commandReturn.outputText.c_str());

    EXPECT_FALSE(commandReturn.errorText.empty());
    File::OSStream_t oss;
    for (std::size_t i = 0; i < commandCall.arguments.size(); i++) {
        oss << i << ": " << commandCall.arguments[i] << std::endl;
    }
    EXPECT_STREQ(oss.str().c_str(), commandReturn.errorText.c_str());
}

TEST_F(Commands, LengthOfFirstArg) {
    commandCall.executable = LengthOfFirstArg_Executable;

    commandCall.arguments = {};
    cc();
    ASSERT_EQ(commandReturn.returnCode, (unsigned char)-1) << "Bad config";

    commandCall.arguments = {""};
    cc();
    EXPECT_EQ(commandReturn.returnCode, 0);

    commandCall.arguments = {"abcde"};
    cc();
    EXPECT_EQ(commandReturn.returnCode, 5);

    commandCall.arguments = {" "};
    cc();
    EXPECT_EQ(commandReturn.returnCode, 1);
}

TEST_F(Commands, LengthOfInput) {
    commandCall.executable = LengthOfInput_Executable;

    commandCall.inputChoice = InputChoice::STRING;
    commandCall.inputString = "abcde";
    cc();
    EXPECT_EQ(commandReturn.returnCode, 5);
}
