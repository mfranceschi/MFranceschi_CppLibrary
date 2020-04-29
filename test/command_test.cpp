//
// Created by mfran on 23/04/2020.
//

#include "tests_datas.hpp"

class Commands : public ::testing::Test {
protected:
    CommandCall commandCall;
    CommandReturn commandReturn;
};

TEST_F(Commands, PWD_Simple) {
    commandCall.executable = File::MakeFilename(true, false, 4, MAKE_FILE_NAME "C:", MAKE_FILE_NAME "cygwin64", MAKE_FILE_NAME "bin", MAKE_FILE_NAME "pwd.exe");// R"(C:\cygwin64\bin\pwd.exe)";
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
}

TEST_F(Commands, PWD_KilledOutput) {
    commandCall.executable = File::MakeFilename(true, false, 4, MAKE_FILE_NAME "C:", MAKE_FILE_NAME "cygwin64", MAKE_FILE_NAME "bin", MAKE_FILE_NAME "pwd.exe");// R"(C:\cygwin64\bin\pwd.exe)";
    commandCall.outputChoice = OutputChoice::KILL;
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
}

TEST_F(Commands, PWD_RetrieveAllOutputs) {
    commandCall.executable = File::MakeFilename(true, false, 4, MAKE_FILE_NAME "C:", MAKE_FILE_NAME "cygwin64", MAKE_FILE_NAME "bin", MAKE_FILE_NAME "pwd.exe");// R"(C:\cygwin64\bin\pwd.exe)";
    commandCall.outputChoice = OutputChoice::RETRIEVE;
    commandCall.errorsChoice = ErrorChoice::RETRIEVE;
    Command(commandCall, commandReturn);
    EXPECT_EQ(0, commandReturn.returnCode);
    EXPECT_FALSE(commandReturn.outputText.empty());
    EXPECT_TRUE(commandReturn.errorText.empty());
}
