# Import the repo according to the pattern described here:
# https://github.com/CLIUtils/cmake/blob/master/AddGoogleTest.cmake
include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             release-1.11.0 )
FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "")
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    unset(CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
endif()

add_library(MF_Commons_Tests STATIC EXCLUDE_FROM_ALL)
target_sources(MF_Commons_Tests PRIVATE main_of_tests.cpp PUBLIC tests_data.hpp)
target_link_libraries(MF_Commons_Tests PUBLIC gtest)
target_include_directories(MF_Commons_Tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
