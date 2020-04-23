//
// Created by mfran on 23/04/2020.
//

#include "tests_datas.hpp"

TEST(Commands, Basic_PWD) {
    CommandCall call;
    call.executable = File::MakeFilename(true, false, 4, MAKE_FILE_NAME "C:", MAKE_FILE_NAME "cygwin64", MAKE_FILE_NAME "bin", MAKE_FILE_NAME "pwd.exe");// R"(C:\cygwin64\bin\pwd.exe)";
    CommandReturn cr;
    Command(call, cr);
    EXPECT_EQ(0, cr.returnCode);
}
