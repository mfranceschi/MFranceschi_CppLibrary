//
// Created by MartinF on 30/05/2025.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_TEST_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_TEST_COMMONS_HPP

#include "MF/Command.hpp"
#include "tests_data.hpp"

#if MF_WINDOWS
#    include "MF/LightWindows.hpp"
#endif

#ifndef TEXT
// Macro used to convert a given string to wide string or not.
// If not defined then we are not using Windows and the 'UNICODE' macro support,
// so this macro does nothing and returns its input.
#    define TEXT(the_string) the_string
#endif

using namespace ::MF::Command;

#if MF_UNICODE
#    include "MF/Strings.hpp"

using CCall = WideCommandCall; /// Type of the CommandCall object to be used.
static const auto& lineEnd = MF::Filesystem::LINE_END_WIDE;

template <typename T>
inline std::wstring toStr(T value) {
    return std::to_wstring(value);
}
#else
using CCall = CommandCall; /// Type of the CommandCall object to be used.
static const auto& lineEnd = MF::Filesystem::LINE_END;

template <typename T>
inline std::string toStr(T value) {
    return std::to_string(value);
}
#    if MF_WINDOWS == 0
using TCHAR = char; /// Character type used in the tests.
#    endif
#endif

static const std::basic_string<TCHAR> HELLO_WORLD_TCHAR = TEXT("Hello, World!");

class CommandTestBase : public ::testing::Test {
   protected:
    CCall commandCall;

    int exitCode = -1;

    void callCommand(int expectedExitCode = 0) {
        exitCode = runCommandAndWait(commandCall).exitCode;
        EXPECT_EQ(expectedExitCode, exitCode);
    }
};

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_TEST_COMMONS_HPP
