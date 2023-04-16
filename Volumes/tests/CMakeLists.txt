
add_executable(MF_Volumes_Tests)
target_link_libraries(
        MF_Volumes_Tests
        PRIVATE
        MF_Volumes
        MF_Commons_Tests
)

target_sources(
        MF_Volumes_Tests
        PRIVATE
        tests.cpp
)

gtest_discover_tests(MF_Volumes_Tests)
