
add_library(MF_System STATIC)

target_sources(MF_System
        PRIVATE
            src/DynamicLibrary.cpp
        PUBLIC
            include/MF/DynamicLibrary.hpp
)
target_link_libraries(MF_System PUBLIC MF_Commons MF_Windows)
target_include_directories(MF_System PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
