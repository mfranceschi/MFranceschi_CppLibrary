
add_library(My_Lib_Shared_4 SHARED lib.cpp)
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
