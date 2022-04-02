
add_library(MF_Windows STATIC EXCLUDE_FROM_ALL)
target_sources(
        MF_Windows
        PUBLIC
        include/MF/LightWindows.hpp
        include/MF/StringSafePlaceHolder.hpp
        include/MF/Windows.hpp
)
target_include_directories(MF_Windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MF_Windows PUBLIC MF_Commons)

if (WIN32)
    target_sources(
            MF_Windows
            PRIVATE
                src/FileAttributes.cpp
                src/Windows.cpp
    )
    if (MF_IN_DEV)
        add_subdirectory(tests)
    endif ()
endif ()
