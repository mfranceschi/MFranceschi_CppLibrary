
add_library(MF_Containers STATIC EXCLUDE_FROM_ALL include/MF/Containers.hpp)
target_include_directories(MF_Containers PUBLIC include)
target_link_libraries(MF_Containers PUBLIC MF_Commons)
target_sources(
        MF_Containers
        PRIVATE
            src/dummy_containers.cpp
        PUBLIC
            include/MF/Array.hpp
            include/MF/FixedLengthVector.hpp
            include/MF/Streams.hpp
)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
