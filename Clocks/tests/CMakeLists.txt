
add_executable(MF_Clocks_Tests)
target_link_libraries(
        MF_Clocks_Tests
        PRIVATE
        MF_Clocks
        MF_Commons_Tests
)

target_sources(
        MF_Clocks_Tests
        PRIVATE
        Clocks_tests.cpp
)

gtest_discover_tests(MF_Clocks_Tests)
