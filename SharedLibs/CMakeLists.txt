
add_library(MF_SharedLibs STATIC src/SharedLib_CanOpen.cpp)

target_sources(MF_SharedLibs
        PRIVATE
            src/SharedLib_DL.cpp
            src/SharedLibs_DL_internals.hpp
            src/SharedLibs_SearchPaths_DL.cpp

            src/SharedLib_Windows.cpp
            src/SharedLibs_SearchPaths_Windows.cpp
        PUBLIC
            include/MF/SharedLibs.hpp
            include/MF/SharedLibsMacros.hpp
)
target_link_libraries(MF_SharedLibs PUBLIC MF_Commons MF_SystemErrors MF_Windows ${CMAKE_DL_LIBS})
target_include_directories(MF_SharedLibs PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
