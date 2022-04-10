
add_executable(MF_SharedLibs_Tests)
target_link_libraries(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SharedLibs
            MF_Commons_Tests
)

target_sources(
        MF_SharedLibs_Tests
        PRIVATE
            shared_libs_tests.cpp
)

add_library(My_Lib_Shared_1 SHARED my_lib_1.hpp my_lib_1.cpp)
target_include_directories(My_Lib_Shared_1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_definitions(My_Lib_Shared_1 PRIVATE MF_EXPORT_ME_1=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_1_NAME="My_Lib_Shared_1"
            MF_SAMPLE_LIB_1_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
)

gtest_discover_tests(MF_SharedLibs_Tests)
