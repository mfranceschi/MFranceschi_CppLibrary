
add_library(MF_Volumes STATIC)

file(GLOB Private_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB Public_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MF/*.hpp)

target_sources(MF_Volumes
        PRIVATE
        ${Private_Sources}
        PUBLIC
        ${Public_Sources}
        )
target_link_libraries(MF_Volumes PUBLIC MF_Commons MF_Windows_Header MF_Filesystem MF_SystemErrors MF_Strings)
target_include_directories(MF_Volumes PUBLIC include)

if (MF_IN_DEV)
    add_subdirectory(tests)
endif ()
