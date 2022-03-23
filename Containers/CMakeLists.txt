
add_library(MF_Containers STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Containers PUBLIC include)
target_link_libraries(MF_Containers PUBLIC MF_Commons)
target_sources(
        MF_Containers
        PRIVATE
            src/hhh.cpp
        PUBLIC
            include/MF/Array.hpp
)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
