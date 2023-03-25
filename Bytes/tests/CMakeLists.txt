
add_executable(MF_Bytes_Tests)
target_link_libraries(
        MF_Bytes_Tests
        PRIVATE
        MF_Bytes
        MF_Commons_Tests
)

target_sources(
        MF_Bytes_Tests
        PRIVATE
        Bytes_tests.cpp
)

gtest_discover_tests(MF_Bytes_Tests)
