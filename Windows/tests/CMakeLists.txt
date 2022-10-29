
add_executable(MF_Windows_Tests)
target_link_libraries(
        MF_Windows_Tests
        PRIVATE
            MF_Windows
            MF_Commons_Tests
)

target_sources(
        MF_Windows_Tests
        PRIVATE
            FileAttributes_tests.cpp
)

gtest_discover_tests(MF_Windows_Tests)

add_executable(EventLog_AT  EventLog_AT.cpp)
target_link_libraries(EventLog_AT MF_Windows)
