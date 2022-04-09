
add_executable(MF_SharedLibs_Tests)
target_link_libraries(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SharedLibs
            MF_Commons_Tests
)

target_sources(
        MF_SharedLibs_Tests
        PRIVATE
            shared_libs_tests.cpp
)

gtest_discover_tests(MF_SharedLibs_Tests)
