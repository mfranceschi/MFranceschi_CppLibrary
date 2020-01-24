cmake_minimum_required(VERSION 3.0)
project(IrregularVerbs LANGUAGES C)
set(CMAKE_C_STANDARD 11)

# set(CURSES_USE_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_THREADSAFE=0")
file(GLOB EXT_LIBRARIES_SOURCE_FILES SQLite/*.c Contiki_2.6/*.c "Low-Level Implementations/*" Texts/*.c)
file(GLOB SOURCE_FILES ./*.c ${EXT_LIBRARIES_SOURCE_FILES})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARY})
