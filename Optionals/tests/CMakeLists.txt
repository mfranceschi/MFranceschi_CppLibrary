
add_executable(MF_Optionals_Tests)
target_link_libraries(
        MF_Optionals_Tests
        PRIVATE
        MF_Optionals
        MF_Commons_Tests
)

target_sources(
        MF_Optionals_Tests
        PRIVATE
        Optionals_tests.cpp
)

gtest_discover_tests(MF_Optionals_Tests)
