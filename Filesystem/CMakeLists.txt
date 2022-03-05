
set(local_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(local_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(MF_Filesystem STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Filesystem PUBLIC ${local_INCLUDE_DIR})
target_link_libraries(MF_Filesystem PUBLIC MF_Commons)
target_sources(
        MF_Filesystem
        PRIVATE
        ${local_SRC_DIR}/Filesystem.cpp
        ${local_SRC_DIR}/FileOpen.cpp
        ${local_SRC_DIR}/FilesystemOSHelper.hpp
        ${local_SRC_DIR}/FilesystemOSHelper.cpp
        ${local_SRC_DIR}/StringSafePlaceHolder.hpp
        ${local_SRC_DIR}/UnixAPIHelper.cpp
        ${local_SRC_DIR}/UnixAPIHelper.hpp
        ${local_SRC_DIR}/WindowsAPIHelper.cpp
        ${local_SRC_DIR}/WindowsAPIHelper.hpp
        PUBLIC
        ${local_INCLUDE_DIR}/MF/Filesystem.hpp
        ${local_INCLUDE_DIR}/MF/FileOpen.hpp
)

add_subdirectory(tests)
