
add_executable(MF_Timezones_Tests)
target_link_libraries(
        MF_Timezones_Tests
        PRIVATE
            MF_Timezones
            MF_Commons_Tests
)

target_sources(
        MF_Timezones_Tests
        PRIVATE
        timezones_tests.cpp
)

gtest_discover_tests(MF_Timezones_Tests)
