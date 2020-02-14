cmake_minimum_required(VERSION 3.0) # I do not know what to put here ^v^
project(IrregularVerbs LANGUAGES C) # C program
set(CMAKE_C_STANDARD 11)            # C11 standard

# Find and include the Curses library.
# It might well be Curses, NCurses or PDCurses. Who knows?
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Make the info of which Curses folder to include available to the program for the #include directive.
if(${CURSES_HAVE_CURSES_H})
    add_compile_definitions(CURSES_INCLUDE_DIRECTORY=curses.h)
elseif(${CURSES_HAVE_NCURSES_H})
    add_compile_definitions(CURSES_INCLUDE_DIRECTORY=ncurses.h)
elseif(${CURSES_HAVE_NCURSES_NCURSES_H})
    add_compile_definitions(CURSES_INCLUDE_DIRECTORY=ncurses/ncurses.h)
elseif(CURSES_HAVE_NCURSES_CURSES_H)
    add_compile_definitions(CURSES_INCLUDE_DIRECTORY=ncurses/curses.h)
endif()

# Preprocessor macro: 1 if we are in debug mode, 0 otherwise.
set(DEBUG_MODE 1)
add_compile_definitions(DEBUG_MODE=${DEBUG_MODE})

# Tell SQLite not to add thread-safety policy + set the
add_compile_definitions(SQLITE_THREADSAFE=0)

# Add sources.
file(GLOB SOURCE_FILES ./*.c utils/*.c Texts/*.c)
add_subdirectory(extlibs)

# Declare the executable.
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${EXTLIBS_SOURCE_FILES})

# Link Curses.
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
