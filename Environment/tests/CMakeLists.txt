
add_executable(MF_Environment_Tests)
target_link_libraries(
        MF_Environment_Tests
        PRIVATE
        MF_Environment
        MF_Commons_Tests
)

target_sources(
        MF_Environment_Tests
        PRIVATE
        Environment_tests.cpp
)

gtest_discover_tests(MF_Environment_Tests)
