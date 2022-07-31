//
// Created by MartinF on 30/07/2022.
//

#include "MF/SystemErrors.hpp"
#include "tests_data.hpp"

using namespace MF::SystemErrors;

/*
ErrorCode_t doSomethingThatSetsLastError() {
#if MF_WINDOWS
    const char* str = "3336402735171707160320";
    auto value = atoll(str);
    return ERANGE;
#else
    const char* terribleString = "0xfffffffffffffffffffffffff";
    long convertedValue = strtol(terribleString, nullptr, 0);
#endif
}

void doSomethingThatDoesNotSetsLastError() {
    const char* terribleString = "0xffff";
    long convertedValue = strtol(terribleString, nullptr, 0);
}

TEST(it_throws_current_system_error_if_true, b) {
    doSomethingThatSetsLastError();

    EXPECT_THROW(throwCurrentSystemErrorIf(true), std::system_error);
}

TEST(GetCurrentErrorCode, it_returns_the_current_error_code) {
    const auto expectedError = doSomethingThatSetsLastError();
    EXPECT_EQ(getCurrentErrorCode(), expectedError);
}

TEST(GetCurrentErrorCode, it_returns_no_error) {
    doSomethingThatDoesNotSetsLastError();
    EXPECT_EQ(getCurrentErrorCode(), 0);
}
*/
