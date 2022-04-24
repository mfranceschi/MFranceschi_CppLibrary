
add_library(My_Lib_Shared_2 SHARED lib.hpp lib.cpp)
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