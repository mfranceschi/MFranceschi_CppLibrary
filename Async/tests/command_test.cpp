//
// Created by mfran on 23/04/2020.
//

#include "MF/Command.hpp"

#include "MF/LightWindows.hpp"
#include "MF/Strings.hpp"
#include "tests_data.hpp"

#ifndef TEXT
#    define TEXT(the_string) the_string
#endif

using namespace ::MF::Command;

#if MF_WINDOWS && UNICODE
using CCall = WideCommandCall;
static const auto lineEnd = MF::Filesystem::LINE_END_WIDE;

template <typename T>
inline std::wstring toStr(T value) {
    return std::to_wstring(value);
}
#else
using CCall = CommandCall;
static const auto lineEnd = MF::Filesystem::LINE_END;

template <typename T>
inline std::string toStr(T value) {
    return std::to_string(value);
}
#endif

static const std::basic_string<TCHAR> HELLO_WORLD = TEXT("Hello, World!");

class CommandTestBase : public ::testing::Test {
   protected:
    CCall commandCall;

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
    commandCall.executable = TEXT(ReturnNbArgs_Executable);
    callCommand(1);
}

TEST_F(CommandTestBase, ReturnNbArgs_OneArg) {
    // Features verified:
    // - We can specify at least 1 argument
    commandCall.executable = TEXT(ReturnNbArgs_Executable);
    commandCall.arguments = {TEXT("arg-one")};
    callCommand(2);
}

TEST_F(CommandTestBase, LengthOfFirstArg) {
    // Features verified:
    // - The arguments passing and maybe parsing is behaving as expected.
    commandCall.executable = TEXT(LengthOfFirstArg_Executable);

    // Empty arg vector -> no argv[1] -> returns 0.
    commandCall.arguments = {};
    callCommand(0);

    // Simple string of length 5 -> returns 5.
    commandCall.arguments = {TEXT("abcde")};
    callCommand(5);

    // Empty/blank string as arg -> should be ignored.
    commandCall.arguments = {TEXT(" ")};
    callCommand(0);

    // String of length 1 but surrounded by escaped double quotes ->
    // the parser receives a string of length 3, but it unwraps the string ->
    // we should end up with only 1.
    commandCall.arguments = {TEXT("\"a\"")};
    callCommand(1);

    // Same as above but it's an edge case where we force a blank string to be passed as arg.
    // The parser unwraps the string -> should return 1;
    commandCall.arguments = {TEXT("\" \"")};
    callCommand(1);
}

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
#if MF_WINDOWS && UNICODE
        MF::Strings::Conversions::ansiToWideChar(Filename_t(test_info_->name()) + ".txt");
#else
        Filename_t(test_info_->name()) + ".txt";
#endif
    commandCall.executable = TEXT(HelloWorld_Executable);
    commandCall.stdOutChoice = makeOutputToFile(filename);
    callCommand();

    std::basic_ifstream<TCHAR> fileStream(filename, std::ios_base::in);
    std::basic_string<TCHAR> line;
    std::getline(fileStream, line);
    fileStream.close();
    EXPECT_EQ(line, HELLO_WORLD);

    MF::Filesystem::deleteFile(filename);
}

TEST_F(CommandTestBase, HelloWorld_OutputToStringStream) {
    // Features verified:
    // - StringStream output works as intended
    std::basic_stringstream<TCHAR> stream;
    commandCall.executable = TEXT(HelloWorld_Executable);
    commandCall.stdOutChoice = makeOutputToIOStream(stream);
    callCommand();

    std::basic_string<TCHAR> line;
    std::getline(stream, line);
    EXPECT_EQ(line, HELLO_WORLD);
}

TEST_F(CommandTestBase, HelloWorld_RetrieveAllOutputs) {
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
    EXPECT_EQ(errStream.str(), TEXT("2") + HELLO_WORLD);
}

TEST_F(CommandTestBase, LengthOfInput_FromString) {
    // Features verified:
    // - String input works as intended
    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputFromString(TEXT("abcde\nfghij\n"));
    callCommand(5);
}

TEST_F(CommandTestBase, LengthOfInput_FromStringStream) {
    // Features verified:
    // - StringStream input works as intended
    std::basic_stringstream<TCHAR> stream;
    stream << "abcde\n"
           << "fghij\n";

    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputFromIOStream(stream);
    callCommand(5);
}

TEST_F(CommandTestBase, LengthOfInput_InputStreamClosed) {
    // Features verified:
    // - Empty input works as intended AND does not cause the child process to be stuck.
    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputEmpty();
    callCommand();
}

TEST_F(CommandTestBase, LengthOfInput_FromFile) {
    // Features verified:
    // - File input works as intended
    commandCall.executable = TEXT(LengthOfInput_Executable);
    commandCall.stdInChoice = makeInputFromFile(TEXT(LOREM_IPSUM_TWO_LINES_FILE));
    callCommand(95);
}

TEST_F(CommandTestBase, OneForEachStream) {
    // This is the Super Smash Bros. of the tests.
    // - We give it arguments. They are basically repeated on StdErr.
    // - We give it some StdIn. It's repeated on StdOut.
    // - We check at the exit code = the number of args, including the executable name.
    commandCall.executable = TEXT(OneForEachStream_Executable);
    commandCall.arguments = {TEXT("one1"), TEXT("two222")};
    const std::basic_string<TCHAR> inputArg = TEXT("input0123");
    commandCall.stdInChoice = makeInputFromString(inputArg);
    std::basic_stringstream<TCHAR> outStream;
    std::basic_stringstream<TCHAR> errStream;
    commandCall.stdOutChoice = makeOutputToIOStream(outStream);
    commandCall.stdErrChoice = makeOutputToIOStream(errStream);

    callCommand(3);

    EXPECT_EQ(3, exitCode);

    auto outString = outStream.str();
    EXPECT_FALSE(outString.empty());
    EXPECT_EQ(inputArg + lineEnd, outString);

    auto errString = errStream.str();
    EXPECT_FALSE(errString.empty());
    std::basic_ostringstream<TCHAR> oss;
    oss << "1: " << commandCall.arguments[0] << lineEnd;
    oss << "2: " << commandCall.arguments[1] << lineEnd;
    EXPECT_STREQ(oss.str().c_str(), errString.c_str());
}

TEST_F(CommandTestBase, GenerateOutput_Ignored) {
    // TODO
    commandCall.executable = TEXT(GenerateOutput_Executable);
    commandCall.arguments = {toStr(2), TEXT("out")};
    commandCall.stdOutChoice = makeOutputIgnored();
    callCommand();
}
