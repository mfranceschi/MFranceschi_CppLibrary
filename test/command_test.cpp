//
// Created by mfran on 23/04/2020.
//

#include "tests_datas.hpp"

class Commands : public ::testing::Test {
protected:
    CommandCall commandCall;
    CommandReturn commandReturn;
    static const File::Filename_t filename;
};
const File::Filename_t Commands::filename = MAKE_FILE_NAME "pwd";

TEST_F(Commands, ReturnNbArgs_Nothing) {
    commandCall.executable = ReturnNbArgs_Executable;
    Command(commandCall, commandReturn);
    EXPECT_EQ(1, commandReturn.returnCode);
}

TEST_F(Commands, PWD_Simple) {
    commandCall.executable = filename;
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_TRUE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, PWD_KilledOutput) {
    commandCall.executable = filename;
    commandCall.outputChoice = OutputChoice::KILL;
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_TRUE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, PWD_RetrieveAllOutputs) {
    commandCall.executable = filename;
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    commandCall.errorChoice = ErrorChoice::RETRIEVE;
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_FALSE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}

TEST_F(Commands, OneForEachStream) {
    commandCall.executable = OneForEachStream_Executable;
    commandCall.arguments = {"one", "two"};
    commandCall.inputChoice = InputChoice::STRING;
    commandCall.inputString = "input0";
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    commandCall.errorChoice = OutputChoice::RETRIEVE;

    Command(commandCall, commandReturn);

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
