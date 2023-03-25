
add_library(MF_Environment STATIC)

file(GLOB Private_Sources
        CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB Public_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MF/*.hpp)

target_sources(MF_Environment
        PRIVATE ${Private_Sources}
        PUBLIC ${Public_Sources}
        )
target_link_libraries(MF_Environment PUBLIC MF_Commons MF_SystemErrors)
target_include_directories(MF_Environment PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
