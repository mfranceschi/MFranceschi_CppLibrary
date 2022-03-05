
add_executable(MF_Filesystem_Tests)
target_link_libraries(
    MF_Filesystem_Tests
    PRIVATE
        MF_Filesystem
        MF_Commons_Tests
)

target_sources(
    MF_Filesystem_Tests
    PRIVATE
        file_tests.cpp
)
