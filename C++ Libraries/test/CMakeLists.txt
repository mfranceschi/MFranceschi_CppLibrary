# Import the repo according to the pattern described here:
# https://github.com/CLIUtils/cmake/blob/master/AddGoogleTest.cmake
include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             release-1.10.0)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "")
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    unset(CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
endif()

set_target_properties(
        gmock gmock_main gtest gtest_main
        PROPERTIES
        EXCLUDE_FROM_ALL 1
        EXCLUDE_FROM_DEFAULT_BUILD 1
)

set(TEST_CASES file_tests.cpp date_tests.cpp)
add_executable(Google_Tests_run)
target_sources(Google_Tests_run PRIVATE ${TEST_CASES})
target_link_libraries(Google_Tests_run gtest MFranceschi_CppLibrary)
