
add_executable(MF_CTime_Tests)
target_link_libraries(
        MF_CTime_Tests
        PRIVATE
        MF_CTime
        MF_Commons_Tests
)

target_sources(
        MF_CTime_Tests
        PRIVATE
        ctime_tests.cpp
)

gtest_discover_tests(MF_CTime_Tests)
