
add_executable(MF_Filesystem_Tests)
target_link_libraries(
        MF_Filesystem_Tests
        PRIVATE
            MF_Filesystem
            MF_Commons_Tests
)

set(MF_FILESYSTEM_TESTS_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/files)
set(MF_FILESYSTEM_TESTS_FILE_SMALL_UTF16LE ${MF_FILESYSTEM_TESTS_FILES_DIR}/Small_utf16le.txt)
set(MF_FILESYSTEM_TESTS_FILE_MEDIUM ${MF_FILESYSTEM_TESTS_FILES_DIR}/aom_v.scx)
set(MF_FILESYSTEM_TESTS_EMPTY_FOLDER ${MF_FILESYSTEM_TESTS_FILES_DIR}/EmptyFolder)
set(MF_FILESYSTEM_TESTS_TEMP ${MF_FILESYSTEM_TESTS_FILES_DIR}/tempFileName_0)
set(MF_FILESYSTEM_TESTS_NON_EXISTING ${MF_FILESYSTEM_TESTS_FILES_DIR}/nonExisting_0)

if(NOT (IS_DIRECTORY ${MF_FILESYSTEM_TESTS_FILES_DIR} AND EXISTS ${MF_FILESYSTEM_TESTS_FILE_SMALL_UTF16LE} AND EXISTS ${MF_FILESYSTEM_TESTS_FILE_MEDIUM}))
    message(FATAL_ERROR "Problem with files for tests")
endif()
if(NOT IS_DIRECTORY ${MF_FILESYSTEM_TESTS_EMPTY_FOLDER})
    file(MAKE_DIRECTORY ${MF_FILESYSTEM_TESTS_EMPTY_FOLDER})
endif()
# We might also want to ensure that MF_FILESYSTEM_TESTS_EMPTY_FOLDER is an empty directory.
if(EXISTS ${MF_FILESYSTEM_TESTS_TEMP})
    file(REMOVE ${MF_FILESYSTEM_TESTS_TEMP})
endif()
if(EXISTS ${MF_FILESYSTEM_TESTS_NON_EXISTING})
    file(REMOVE ${MF_FILESYSTEM_TESTS_NON_EXISTING})
endif()

target_compile_definitions(
        MF_Filesystem_Tests
        PRIVATE
            MF_FILESYSTEM_TESTS_FILES_DIR="${MF_FILESYSTEM_TESTS_FILES_DIR}"
            MF_FILESYSTEM_TESTS_FILE_SMALL_UTF16LE="${MF_FILESYSTEM_TESTS_FILE_SMALL_UTF16LE}"
            MF_FILESYSTEM_TESTS_FILE_MEDIUM="${MF_FILESYSTEM_TESTS_FILE_MEDIUM}"
            MF_FILESYSTEM_TESTS_EMPTY_FOLDER="${MF_FILESYSTEM_TESTS_EMPTY_FOLDER}"
            MF_FILESYSTEM_TESTS_TEMP="${MF_FILESYSTEM_TESTS_TEMP}"
            MF_FILESYSTEM_TESTS_NON_EXISTING="${MF_FILESYSTEM_TESTS_NON_EXISTING}"
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
