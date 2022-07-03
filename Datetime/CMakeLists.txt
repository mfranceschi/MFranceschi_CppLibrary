
add_library(MF_Datetime STATIC)

file(GLOB Private_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB Public_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MF/*.hpp)

target_sources(MF_Datetime
        PRIVATE
        ${Private_Sources}
        PUBLIC
        ${Public_Sources})
target_link_libraries(MF_Datetime PUBLIC MF_Commons MF_Windows MF_CTime)
target_include_directories(MF_Datetime PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
