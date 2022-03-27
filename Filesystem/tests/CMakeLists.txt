
add_executable(MF_Filesystem_Tests)
target_link_libraries(
        MF_Filesystem_Tests
        PRIVATE
            MF_Filesystem
            MF_Commons_Tests
)
target_compile_definitions(
        MF_Filesystem_Tests
        PRIVATE
            FILES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/files"
)

target_sources(
        MF_Filesystem_Tests
        PRIVATE
            file_tests.cpp
            Filesystem_tests_commons.hpp
            Filesystem_ListFilesInDirectory_tests.cpp
            Filesystem_ReadWholeFile_tests.cpp
)

gtest_discover_tests(MF_Filesystem_Tests)
