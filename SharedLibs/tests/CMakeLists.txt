
add_executable(MF_SharedLibs_Tests can_open_tests.cpp)
target_link_libraries(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SharedLibs
            MF_Commons_Tests
            MF_Filesystem
)

target_sources(
        MF_SharedLibs_Tests
        PRIVATE
            shared_libs_tests_common.hpp
            shared_libs_tests.cpp
            my_lib_2_tests.cpp
            my_lib_3_tests.cpp
            my_lib_4_tests.cpp
            my_lib_5_tests.cpp
)

add_subdirectory(my_lib_1_empty)
add_subdirectory(my_lib_2_functions)
add_subdirectory(my_lib_3_variables_and_init)
add_subdirectory(my_lib_4_empty_load_and_unload)
add_subdirectory(my_lib_5_load_throws)

gtest_discover_tests(MF_SharedLibs_Tests)
