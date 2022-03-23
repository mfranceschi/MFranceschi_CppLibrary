
add_executable(MF_System_Tests)
target_link_libraries(
        MF_System_Tests
        PRIVATE
            MF_System
            MF_Commons_Tests
)

target_sources(
        MF_System_Tests
        PRIVATE
            date_tests.cpp
)

gtest_discover_tests(MF_System_Tests)
