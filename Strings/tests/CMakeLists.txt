
add_executable(MF_Strings_Tests)
target_link_libraries(
        MF_Strings_Tests
        PRIVATE
        MF_Strings
        MF_Commons_Tests
)

target_sources(
        MF_Strings_Tests
        PRIVATE
        Strings_tests.cpp
)

gtest_discover_tests(MF_Strings_Tests)
