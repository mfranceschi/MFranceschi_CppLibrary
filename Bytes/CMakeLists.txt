
add_library(MF_Bytes STATIC)

file(GLOB Private_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB Public_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MF/*.hpp)

target_sources(MF_Bytes
        PRIVATE ${Private_Sources}
        PUBLIC ${Public_Sources}
        )
target_include_directories(MF_Bytes PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
