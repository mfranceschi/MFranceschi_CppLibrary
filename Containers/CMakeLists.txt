
add_library(MF_Containers STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Containers PUBLIC include)
target_link_libraries(MF_Containers PUBLIC MF_Commons)
target_sources(
        MF_Containers
        PRIVATE
        PUBLIC
            include/MF/Array.hpp
)

add_executable(MF_Containers_Main hhh.cpp)
target_link_libraries(MF_Containers_Main PUBLIC MF_Containers MF_Commons_Tests)
