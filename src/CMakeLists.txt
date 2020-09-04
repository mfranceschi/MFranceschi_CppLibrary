
if (MINGW)
    set_source_files_properties(WindowsAPIHelper.cpp PROPERTIES COMPILE_FLAGS "-Wformat=0 -Wno-sign-compare")
endif()

add_library(${MF_Lib_Libname} STATIC)
target_sources(${MF_Lib_Libname}
        PRIVATE
        Command.cpp CommandHelper_Generic.cpp CommandHelper_Windows.cpp CommandHelper_Unix.cpp CommandHelper.hpp
        Date.cpp
        DynamicLibrary.cpp
        File.cpp
        FileOpen.cpp
        GeoCoord.cpp
        Toolbox.cpp
        WindowsAPIHelper.cpp WindowsAPIHelper.hpp
        UnixAPIHelper.cpp UnixAPIHelper.hpp
        StringSafePlaceHolder.hpp
        Security.cpp

        PUBLIC
        Array.hpp
        Command.hpp
        Date.hpp
        DynamicLibrary.hpp
        File.hpp
        FileOpen.hpp
        GeoCoord.hpp
        Toolbox.hpp
        Security.hpp
        )
target_include_directories(${MF_Lib_Libname} PUBLIC ../includes)

if (MSVC)
    target_compile_options(${MF_Lib_Libname} PRIVATE /W3)
else()
    target_compile_options(${MF_Lib_Libname} PRIVATE -Wall -Wextra)
endif()

set(MFranceschi_CppLibrary_LinkLibs "")
if (Threads_FOUND)
    STRING(APPEND MFranceschi_CppLibrary_LinkLibs "${CMAKE_THREAD_LIBS_INIT}")
endif()
target_link_libraries(MFranceschi_CppLibrary PUBLIC
        ${MFranceschi_CppLibrary_LinkLibs})

add_executable(MF_Lib_demo main.cpp)
target_link_libraries(MF_Lib_demo PRIVATE "${MF_Lib_Libname}")
