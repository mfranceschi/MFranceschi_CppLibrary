
add_executable(MF_SystemErrors_Tests SystemErrors_TestHelper.hpp)
target_link_libraries(
        MF_SystemErrors_Tests
        PRIVATE
        MF_SystemErrors
        MF_Commons_Tests
)

target_sources(
        MF_SystemErrors_Tests
        PRIVATE
        system_errors_errno_tests.cpp
        system_errors_wsa_tests.cpp
        system_errors_win32_tests.cpp
        SystemErrors_TestHelper.hpp
)

gtest_discover_tests(MF_SystemErrors_Tests)
