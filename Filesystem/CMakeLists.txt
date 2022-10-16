
add_library(MF_Filesystem STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Filesystem PUBLIC include)
target_link_libraries(MF_Filesystem PUBLIC MF_Commons MF_Windows MF_SystemErrors MF_Strings)
target_sources(
        MF_Filesystem
        PRIVATE
            src/Filesystem.cpp
            src/Filesystem_Windows.cpp
            src/Filesystem_Windows_ReadWholeFile.cpp
            src/Filesystem_Constants.cpp
            src/FilesystemOSHelper.hpp
            src/FilesystemOSHelper_Unix.cpp
            src/FilesystemOSHelper_Windows.cpp
        PUBLIC
            include/MF/Filesystem.hpp
)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
