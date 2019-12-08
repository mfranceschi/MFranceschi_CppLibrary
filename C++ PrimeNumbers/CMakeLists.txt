# CMake configuration
cmake_minimum_required(VERSION 3.0)
project(testet LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

# Settings for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS REQUIRED Widgets)  # Ensure Qt is available

# The executable
file(GLOB SOURCE_FILES ./*.cpp)
if (WIN32)
    add_executable(PrimeNumbers WIN32 ${SOURCE_FILES})
else()
    add_executable(PrimeNumbers ${SOURCE_FILES})
endif()
target_link_libraries(PrimeNumbers Qt5::Widgets)
