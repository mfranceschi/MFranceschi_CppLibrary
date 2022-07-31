//
// Created by MartinF on 30/07/2022.
//

#include "MF/SystemErrors.hpp"
#include "tests_data.hpp"

using namespace MF::SystemErrors::Errno;

static_assert(
    sizeof(int) <= 4, "Tests must be adapted because 'int' has a bigger size than anticipated.");

static ErrorCode_t doSomethingThatSetsLastError() {
    // This overflowing number is taken from
    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/atoll-atoll-l-wtoll-wtoll-l?view=msvc-170#example
    const std::string str = "3336402735171707160320";

    const auto result = atoi(str.c_str());
    EXPECT_EQ(result, INT_MAX);

    return ERANGE;
}

static void doSomethingThatDoesNotSetsLastError() {
    const std::string str = std::to_string(42);
    int convertedValue = atoi(str.c_str());
}

TEST(Errno_ThrowCurrentSystemErrorIf, it_throws_if_true) {
    const auto expectedError = doSomethingThatSetsLastError();

    try {
        throwCurrentSystemErrorIf(true);
        FAIL() << "Expected an exception to be thrown.";
    } catch (const std::system_error& systemError) {
        EXPECT_EQ(systemError.code().value(), expectedError);
        EXPECT_GT(std::strlen(systemError.what()), 1);
    } catch (...) {
        FAIL() << "Unexpected or unknown error.";
    }
}

TEST(Errno_ThrowCurrentSystemErrorIf, it_does_not_throw_if_false) {
    const auto expectedError = doSomethingThatSetsLastError();

    EXPECT_NO_THROW(throwCurrentSystemErrorIf(false));
}

TEST(Errno_GetCurrentErrorCode, it_returns_the_current_error_code) {
    const auto expectedError = doSomethingThatSetsLastError();

    const auto currentError = getCurrentErrorCode();
    EXPECT_EQ(currentError, expectedError);
}

TEST(Errno_GetCurrentErrorCode, it_returns_no_error) {
    constexpr int myValue = 4242;
    setCurrentErrorCode(myValue);
    doSomethingThatDoesNotSetsLastError();

    const auto currentError = getCurrentErrorCode();
    EXPECT_EQ(currentError, myValue);
}
