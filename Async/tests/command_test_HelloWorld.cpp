//
// Created by MartinF on 30/05/2025.
//

#include <cstring>

#include "command_test_commons.hpp"

TEST_F(CommandTestBase, HelloWorld_OutputIgnored) {
    // Features verified:
    // - Ignored output does not break
    // NOTE: this does NOT check that nothing was written to the console.
    commandCall.executable = TEXT(HelloWorld_Executable);
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}

TEST_F(CommandTestBase, HelloWorld_OutputToConsole) {
    // Features verified:
    // - Console output does not break
    // NOTE: this does NOT check that something was written to the console.
    commandCall.executable = TEXT(HelloWorld_Executable);
    commandCall.stdOutChoice = makeOutputToConsole();
    callCommand();
}

TEST_F(CommandTestBase, HelloWorld_OutputToFile) {
    // Features verified:
    // - File output works as intended
    const std::basic_string<TCHAR> filename =
        TEXT(CMAKE_CURRENT_BINARY_DIR) +
#if MF_UNICODE
        MF::Strings::Conversions::ansiToWideChar(Filename_t(test_info_->name()) + ".txt");
#else
        Filename_t(test_info_->name()) + ".txt";
#endif
    commandCall.executable = TEXT(HelloWorld_Executable);
    commandCall.stdOutChoice = makeOutputToFile(filename);
    callCommand();

    {
        auto wholeFile = MF::Filesystem::readWholeFile(filename);
        EXPECT_GT(wholeFile->getSize(), 0) << "Resulting file is empty!";
        EXPECT_EQ(
            std::memcmp(wholeFile->getContent(), HELLO_WORLD_TCHAR.data(), wholeFile->getSize()), 0)
            << "File content (size=" << wholeFile->getSize() << "): " << wholeFile->getContent();
    }

    MF::Filesystem::deleteFile(filename);
}

#if 0 // TODO: this test is disabled because it does not work correctly on Unix.
TEST_F(CommandTestBase, DISABLED_HelloWorld_OutputToStringStream) {
    // Features verified:
    // - StringStream output works as intended
    std::basic_stringstream<TCHAR> stream;
    commandCall.executable = TEXT(HelloWorld_Executable);
    commandCall.stdOutChoice = makeOutputToIOStream(stream);
    callCommand();

    std::basic_string<TCHAR> line;
    std::getline(stream, line);
    EXPECT_EQ(line, HELLO_WORLD_TCHAR);
}

TEST_F(CommandTestBase, DISABLED_HelloWorld_RetrieveAllOutputs) {
    // Features verified:
    // - Stream redirection is distinct between StdOut and StdErr
    // - No output = nothing is written in the corresponding StringStream
    commandCall.executable = TEXT(HelloWorldToStderr_Executable);
    std::basic_stringstream<TCHAR> outStream;
    outStream << "1";
    commandCall.stdOutChoice = makeOutputToIOStream(outStream);
    std::basic_stringstream<TCHAR> errStream;
    errStream << TEXT("2");
    commandCall.stdErrChoice = makeOutputToIOStream(errStream);

    callCommand();

    EXPECT_EQ(outStream.str(), TEXT("1"));
    EXPECT_EQ(errStream.str(), TEXT("2") + HELLO_WORLD_TCHAR);
}
#endif
