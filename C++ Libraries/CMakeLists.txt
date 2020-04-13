cmake_minimum_required(VERSION 3.15)
project(MyWorks_test0)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)
include_directories(.)
add_executable(MyWorks_test0 ${SOURCES} WindowsAPIHelper.cpp WindowsAPIHelper.hpp)

if (WIN32)
    # add_compile_definitions(UNICODE)
    # target_compile_definitions(MyWorks_test0 PUBLIC _CRT_SECURE_NO_WARNINGS=)
    target_compile_options(MyWorks_test0 PUBLIC /W3)
    target_link_libraries(MyWorks_test0 "Shlwapi.dll")
else()
    target_compile_options(MyWorks_test0 PRIVATE -Wall -Wextra)
endif()
add_subdirectory(Google_tests)
