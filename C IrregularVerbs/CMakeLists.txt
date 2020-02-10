cmake_minimum_required(VERSION 3.0) # I do not know what to put here ^v^
project(IrregularVerbs LANGUAGES C) # C program
set(CMAKE_C_STANDARD 11)            # C11 standard

# Find and include the Curses library.
# It might well be Curses, NCurses or PDCurses. Who knows?
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Preprocessor macro: 1 if we are in debug mode, 0 otherwise.
set(DEBUG_MODE 1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_MODE=${DEBUG_MODE}")

# Tell SQLite not to add thread-safety policy + set the
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_THREADSAFE=0")

# Add sources.
file(GLOB SOURCE_FILES ./*.c utils/*.c Texts/*.c)
add_subdirectory(extlibs)

# Declare the executable.
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${EXTLIBS_SOURCE_FILES})

# Link Curses.
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
