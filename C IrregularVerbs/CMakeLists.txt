cmake_minimum_required(VERSION 3.0)
project(IrregularVerbs LANGUAGES C)
set(CMAKE_C_STANDARD 11)

set(CURSES_USE_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

file(GLOB SQLITE_SOURCE_FILES ./SQLite/*.c)
file(GLOB SOURCE_FILES ./*.c ${SQLITE_SOURCE_FILES})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARY})
