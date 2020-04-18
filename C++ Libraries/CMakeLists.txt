cmake_minimum_required(VERSION 3.15)
project(MFranceschi_CppLibraries
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(includes)
add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME Testing_MFranceschi_CppLibraries COMMAND Google_Tests_run)
