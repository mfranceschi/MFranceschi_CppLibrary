
add_library(MF_SharedLibs STATIC src/SharedLib_Windows.cpp)

target_sources(MF_SharedLibs
        PRIVATE
            src/SharedLibs.cpp
        PUBLIC
            include/MF/SharedLibs.hpp
            include/MF/SharedLibsMacros.hpp
)
target_link_libraries(MF_SharedLibs PUBLIC MF_Commons MF_Windows)
target_include_directories(MF_SharedLibs PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
