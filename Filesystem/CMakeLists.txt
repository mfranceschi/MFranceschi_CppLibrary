
add_library(MF_Filesystem STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Filesystem PUBLIC include)
target_link_libraries(MF_Filesystem PUBLIC MF_Commons MF_Windows)
target_sources(
        MF_Filesystem
        PRIVATE
            src/Filesystem.cpp
            src/FileRead.cpp
            src/FilesystemOSHelper.hpp
            src/FilesystemOSHelper_Unix.cpp
            src/FilesystemOSHelper_Windows.cpp
        PUBLIC
            include/MF/Filesystem.hpp
)

add_subdirectory(tests)
