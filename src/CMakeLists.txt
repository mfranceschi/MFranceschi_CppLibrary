set(My_Sources
        Command.cpp Command.hpp
        Date.cpp Date.hpp
        File.cpp File.hpp
        FileOpen.cpp FileOpen.hpp
        GeoCoord.cpp GeoCoord.hpp
        Toolbox.cpp Toolbox.hpp
        WindowsAPIHelper.cpp WindowsAPIHelper.hpp
        UnixAPIHelper.cpp UnixAPIHelper.hpp
        StringSafePlaceHolder.hpp
        CommandHelper_Generic.cpp CommandHelper_Windows.cpp CommandHelper.hpp
        Security.cpp Security.hpp
        )

if (MINGW)
    set_source_files_properties(WindowsAPIHelper.cpp PROPERTIES COMPILE_FLAGS "-Wformat=0 -Wno-sign-compare")
endif()

add_library(${MF_Lib_Libname} STATIC)
target_sources(${MF_Lib_Libname} PRIVATE ${My_Sources})

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
