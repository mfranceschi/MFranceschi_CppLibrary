set(My_Sources
        Command.cpp Command.hpp
        Date.cpp Date.hpp
        File.cpp File.hpp
        GeoCoord.cpp GeoCoord.hpp
        # TimingExperience.cpp TimingExperience.hpp
        Toolbox.cpp Toolbox.hpp
        WindowsAPIHelper.cpp WindowsAPIHelper.hpp)

add_library(MFranceschi_CppLibrary
        STATIC UnixAPIHelper.cpp UnixAPIHelper.hpp StringSafePlaceHolder.hpp)
target_sources(MFranceschi_CppLibrary PRIVATE ${My_Sources})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

if (WIN32)
    # add_compile_definitions(UNICODE)
    if (MSVC)
        target_compile_options(MFranceschi_CppLibrary PRIVATE /W3)
    else()
        target_compile_options(MFranceschi_CppLibrary PRIVATE -Wall -Wextra)
    endif()
    target_link_libraries(MFranceschi_CppLibrary PUBLIC Threads::Threads "Shlwapi.dll")
else()
    target_compile_options(MFranceschi_CppLibrary PRIVATE -Wall -Wextra)
endif()

add_executable(demo main.cpp)
target_link_libraries(demo MFranceschi_CppLibrary)
