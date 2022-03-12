
add_executable(MF_Containers_Tests)
target_link_libraries(
        MF_Containers_Tests
        PUBLIC
            MF_Commons_Tests
            MF_Containers
)
target_sources(
        MF_Containers_Tests
        PRIVATE
            array_tests.cpp
)

include(GoogleTest)
gtest_discover_tests(MF_Containers_Tests)
