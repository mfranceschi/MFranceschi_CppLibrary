# THINGS REQUIRED
# - CMake 3.15 or +
# - C++14
# - A Threading library (for `find_package(Threads REQUIRED)`).
# CMAKE VARIABLES TO USE
# - MF_IN_DEV: enables testing.
#   Will include and build unit tests.
#   They use Google Test (it's Git Clone'd if needed) and it requires Python 3.8+.
# - UNICODE or MF_UNICODE: adds the compile definition "UNICODE=1" to the lib.
#   Not needed if it's already done by your project.
# COMPILE DEFINITIONS THAT HAVE AN IMPACT HERE
# - UNICODE: for MSVC, uses the UNICODE version of Win32 functions
#   (wchar_t instead of char). It's all internal so that the end-user
#   doesn't have to adapt the function calls for this lib.

cmake_minimum_required(VERSION 3.15)
project(MFranceschi_CppLibraries
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

option(MF_IN_DEV "If true then also builds the testing framework." OFF)
option(MF_UNICODE "If true then adds the compile definition 'UNICODE=1'." OFF)

# ----- GET THE THREADS LIBRARY ----- #
find_package(Threads REQUIRED)
#[[
if(NOT Threads_FOUND)
    if(MINGW AND NOT MF_Lib_ForceUseThreads)
        # The user does not want to force the use of a threading library.
        # In MinGW it does not work fine, so we force a dummy non-detection of the libraries.
        set(Threads_FOUND FALSE)
        set(CMAKE_THREAD_LIBS_INIT "")
    else()
        find_package(Threads)
    endif()
endif()
add_compile_definitions(
        Threads_FOUND=$<BOOL:${Threads_FOUND}>
        # I_Want_Mem_Leaks=1
)
]]

# ----- WINDOWS UNICODE ----- #
if(MF_UNICODE OR UNICODE)
    add_compile_definitions(UNICODE=1)
endif()

# ----- DEV AND TESTS SETUP ----- #
if(MF_IN_DEV)
    if(MSVC)
        # warning level 4
        add_compile_options(/W4)
    else()
        # lots of warnings
        add_compile_options(-Wall -Wextra -pedantic)
    endif()

    enable_testing()

    set(_MF_GTEST_GIT_URL https://github.com/google/googletest)

    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)

        #cmake-lint: disable=readability/wonkycase
        FetchContent_Declare(
                googletest
                URL ${_MF_GTEST_GIT_URL}/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    include(GoogleTest)
endif()

add_subdirectory(Commons)
add_subdirectory(Windows)
add_subdirectory(Environment)
add_subdirectory(Filesystem)
add_subdirectory(SharedLibs)
add_subdirectory(Datetime)
# add_subdirectory(Async) # TODO - not ready yet
add_subdirectory(Containers)
add_subdirectory(Timezones)

add_library(MF_ALL INTERFACE)
target_link_libraries(
        MF_ALL
        INTERFACE
            MF_Commons
            MF_Filesystem
            MF_Windows
            # MF_Async
            MF_SharedLibs
)

add_subdirectory(misc/TimingExperience)
