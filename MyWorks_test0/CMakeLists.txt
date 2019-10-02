cmake_minimum_required(VERSION 3.15)
project(MyWorks_test0)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)
include_directories(.)
add_executable(MyWorks_test0 main.cpp ${SOURCES})
add_subdirectory(Google_tests)
