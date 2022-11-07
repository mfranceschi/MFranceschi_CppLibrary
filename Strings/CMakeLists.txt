
add_library(MF_Strings STATIC src/Strings_Chars.cpp)

file(GLOB Private_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB Public_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MF/*.hpp)

target_sources(MF_Strings
        PRIVATE ${Private_Sources}
        PUBLIC ${Public_Sources}
        )
target_link_libraries(MF_Strings PUBLIC MF_Windows_Header)
target_include_directories(MF_Strings PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
