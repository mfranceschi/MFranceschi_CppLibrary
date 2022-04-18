
add_executable(MF_SharedLibs_Tests )
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
            shared_libs_tests.cpp
            my_lib_2_tests.cpp
            my_lib_3_tests.cpp
            my_lib_4_tests.cpp
            my_lib_5_tests.cpp
)

add_library(My_Lib_Shared_1 SHARED my_lib_1_empty.hpp my_lib_1_empty.cpp)
target_link_libraries(My_Lib_Shared_1 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_1 PRIVATE MF_EXPORT_ME_1=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_1_NAME="My_Lib_Shared_1"
            MF_SAMPLE_LIB_1_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
            MF_EXPORT_ME_1=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_1)

add_library(My_Lib_Shared_2 SHARED my_lib_2.hpp my_lib_2.cpp)
target_link_libraries(My_Lib_Shared_2 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_2 PRIVATE MF_EXPORT_ME_2=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_2_NAME="My_Lib_Shared_2"
            MF_SAMPLE_LIB_2_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
            MF_EXPORT_ME_2=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_2)

add_library(My_Lib_Shared_3 SHARED my_lib_3_variables_and_init.hpp  my_lib_3_variables_and_init.cpp)
target_link_libraries(My_Lib_Shared_3 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_3 PRIVATE MF_EXPORT_ME_3=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_3_NAME="My_Lib_Shared_3"
            MF_SAMPLE_LIB_3_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
            MF_EXPORT_ME_3=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_3)

add_library(My_Lib_Shared_4 SHARED my_lib_4_empty_load_and_unload.cpp)
target_link_libraries(My_Lib_Shared_4 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_4 PRIVATE MF_EXPORT_ME_4=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_4_NAME="My_Lib_Shared_4"
            MF_SAMPLE_LIB_4_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
            MF_EXPORT_ME_4=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_4)

add_library(My_Lib_Shared_5 SHARED my_lib_5_load_throws.cpp)
target_link_libraries(My_Lib_Shared_5 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_5 PRIVATE MF_EXPORT_ME_5=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_5_NAME="My_Lib_Shared_5"
            MF_SAMPLE_LIB_5_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
            MF_EXPORT_ME_5=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_5)

gtest_discover_tests(MF_SharedLibs_Tests)
