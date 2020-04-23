cmake_minimum_required(VERSION 3.15)
project(MFranceschi_CppLibraries
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

if (NOT Threads_FOUND)
    if ((MINGW) AND (NOT MF_Lib_ForceUseThreads))
        # The user does not want to force the use of a threading library.
        # In MinGW it does not work fine, so we force a dummy non-detection of the libraries.
        set(Threads_FOUND FALSE)
        set(CMAKE_THREAD_LIBS_INIT "")
    else()
        find_package(Threads)
    endif()
endif()
add_compile_definitions(Threads_FOUND=$<BOOL:${Threads_FOUND}>)

if (MF_Lib_WantUnicode OR UNICODE)
    add_compile_definitions(UNICODE=1)
endif()

set(MF_Lib_Libname MFranceschi_CppLibrary)

include_directories(includes)
add_subdirectory(src)

if (NOT MF_Lib_RemoveTests)
    enable_testing()
    if (NOT MINGW)
        # Unfortunately, the designated version of Google Test has a #include issue with MinGW.
        add_subdirectory(test)
        add_test(NAME Testing_MFranceschi_CppLibraries COMMAND Google_Tests_run)
    endif()
    add_subdirectory(TimingExperience)
endif()
