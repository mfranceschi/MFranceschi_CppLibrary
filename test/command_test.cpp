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
    commandCall.errorsChoice = ErrorChoice::RETRIEVE;
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_FALSE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}
