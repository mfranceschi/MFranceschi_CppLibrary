cmake_minimum_required(VERSION 3.15)
project(MFranceschi_CppLibraries
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

# ----- GET THE THREADS LIBRARY ----- #
if (NOT Threads_FOUND)
    if (MINGW AND NOT MF_Lib_ForceUseThreads)
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

# ----- WINDOWS UNICODE ----- #
if (MF_Lib_WantUnicode OR UNICODE)
    add_compile_definitions(UNICODE=1)
endif()

enable_testing()

add_subdirectory(Commons)
add_subdirectory(Windows)

add_subdirectory(Filesystem)
add_subdirectory(System)
add_subdirectory(Async)
add_subdirectory(Containers)

add_library(MF_ALL INTERFACE)
target_link_libraries(
        MF_ALL
        INTERFACE
            MF_Commons
            MF_Filesystem
            MF_Windows
            MF_Async
            MF_System
)
