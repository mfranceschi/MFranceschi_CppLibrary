
add_executable(MF_SystemErrors_Tests)
target_link_libraries(
        MF_SystemErrors_Tests
        PRIVATE
        MF_SystemErrors
        MF_Commons_Tests
)

target_sources(
        MF_SystemErrors_Tests
        PRIVATE
        system_errors_tests.cpp
        system_errors_win32_tests.cpp
        system_errors_errno_tests.cpp
)

gtest_discover_tests(MF_SystemErrors_Tests)
