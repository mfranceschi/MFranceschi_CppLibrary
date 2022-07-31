//
// Created by MartinF on 31/07/2022.
//

#if MF_WINDOWS && MF_SystemErrors_WSA

#    include <array>

#    include "MF/SystemErrors.hpp"
#    include "WinSock2.h"
#    include "tests_data.hpp"

#    pragma comment(lib, "Ws2_32.lib")

using namespace MF::SystemErrors::Wsa;

static ErrorCode_t doSomethingThatSetsLastError() {
    std::array<char, 50> buffer;
    const int result = gethostname(buffer.data(), buffer.size());

    EXPECT_EQ(result, SOCKET_ERROR);

    return WSANOTINITIALISED;
}

static void doSomethingThatDoesNotSetsLastError() {
    WSADATA wsadata;
    ASSERT_EQ(WSAStartup(MAKEWORD(2, 2), &wsadata), 0);

    std::array<char, 50> buffer;
    const int result = gethostname(buffer.data(), buffer.size());

    EXPECT_EQ(result, 0);

    ASSERT_EQ(WSACleanup(), 0);
}

TEST(Wsa_ThrowCurrentSystemErrorIf, it_throws_if_true) {
    const auto expectedError = doSomethingThatSetsLastError();

    try {
        throwCurrentSystemErrorIf(true);
        FAIL() << "Expected an exception to be thrown.";
    } catch (const std::system_error& systemError) {
        EXPECT_EQ(systemError.code().value(), expectedError);
        EXPECT_GT(std::strlen(systemError.what()), 1) << systemError.what();
    } catch (...) {
        FAIL() << "Unexpected or unknown error.";
    }
}

TEST(Wsa_ThrowCurrentSystemErrorIf, it_does_not_throw_if_false) {
    doSomethingThatSetsLastError();

    EXPECT_NO_THROW(throwCurrentSystemErrorIf(false));
}

TEST(Wsa_GetCurrentErrorCode, it_returns_the_current_error_code) {
    const auto expectedError = doSomethingThatSetsLastError();

    const auto currentError = getCurrentErrorCode();
    EXPECT_EQ(currentError, expectedError);
}

TEST(Wsa_GetCurrentErrorCode, it_returns_no_error) {
    constexpr int myValue = 4242;
    setCurrentErrorCode(myValue);
    doSomethingThatDoesNotSetsLastError();

    const auto currentError = getCurrentErrorCode();
    // The WSA last error appears to be reset to zero when calling the function getHostName.
    EXPECT_TRUE(currentError == myValue || currentError == 0);
}

#endif
