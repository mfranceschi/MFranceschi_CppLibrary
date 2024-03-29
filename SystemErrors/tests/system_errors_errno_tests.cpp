//
// Created by MartinF on 30/07/2022.
//

#include "MF/SystemErrors.hpp"
#include "SystemErrors_TestHelper.hpp"
#include "tests_data.hpp"

using namespace MF::SystemErrors;
using ErrorCode_t = Errno::ErrorCode_t;

static_assert(
    sizeof(int) <= 4, "Tests must be adapted because 'int' has a bigger size than anticipated.");

static ErrorCode_t doSomethingThatSetsLastError() {
    // This overflowing number is taken from
    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/atoll-atoll-l-wtoll-wtoll-l?view=msvc-170#example
    const std::string str = "3336402735171707160320";

    const auto result = strtol(str.c_str(), nullptr, 10);
    EXPECT_EQ(result, LONG_MAX);

    return ERANGE; // "Numerical result out of range" (Linux) or "Result too large" (Windows)
}

static void doSomethingThatDoesNotSetsLastError() {
    const std::string str = std::to_string(42);
    int convertedValue = atoi(str.c_str());
}

TEST(Errno_ThrowCurrentSystemErrorIf, it_throws_if_true) {
    const auto expectedError = doSomethingThatSetsLastError();

    try {
        Errno::throwCurrentSystemErrorIf(true);
        FAIL() << "Expected an exception to be thrown.";
    } catch (const SystemError& systemError) {
        checkSystemError(systemError, Paradigm::Errno, expectedError);
    }
}

TEST(Errno_ThrowCurrentSystemErrorIf, it_does_not_throw_if_false) {
    const auto expectedError = doSomethingThatSetsLastError();

    EXPECT_NO_THROW(Errno::throwCurrentSystemErrorIf(false));
}

TEST(Errno_GetCurrentErrorCode, it_returns_the_current_error_code) {
    const auto expectedError = doSomethingThatSetsLastError();

    const auto currentError = Errno::getCurrentErrorCode();
    EXPECT_EQ(currentError, expectedError);
}

TEST(Errno_GetCurrentErrorCode, it_returns_no_error) {
    constexpr int myValue = 4242;
    Errno::setCurrentErrorCode(myValue);
    const auto currentError = Errno::getCurrentErrorCode();
    EXPECT_EQ(currentError, myValue);
}
