set(My_Sources
        Command.cpp Command.hpp
        Date.cpp Date.hpp
        File.cpp File.hpp
        GeoCoord.cpp GeoCoord.hpp
        TimingExperience.cpp TimingExperience.hpp
        Toolbox.cpp Toolbox.hpp
        WindowsAPIHelper.cpp WindowsAPIHelper.hpp)

add_library(MFranceschi_CppLibrary
        STATIC UnixAPIHelper.cpp UnixAPIHelper.hpp)
target_sources(MFranceschi_CppLibrary PRIVATE ${My_Sources})

if (WIN32)
    # add_compile_definitions(UNICODE)
    target_compile_options(MFranceschi_CppLibrary PUBLIC /W3)
    target_link_libraries(MFranceschi_CppLibrary "Shlwapi.dll")
else()
    target_compile_options(MFranceschi_CppLibrary PRIVATE -Wall -Wextra)
endif()

add_executable(demo main.cpp)
target_link_libraries(demo MFranceschi_CppLibrary)
