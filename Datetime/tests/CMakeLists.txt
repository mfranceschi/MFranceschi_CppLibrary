
add_executable(MF_Datetime_Tests)
target_link_libraries(
        MF_Datetime_Tests
        PRIVATE
            MF_Datetime
            MF_Commons_Tests
)

target_sources(
        MF_Datetime_Tests
        PRIVATE
            date_tests.cpp
)

gtest_discover_tests(MF_Datetime_Tests)
