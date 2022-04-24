
add_library(My_Lib_Shared_1 SHARED lib.hpp lib.cpp)
target_link_libraries(My_Lib_Shared_1 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_1 PRIVATE MF_EXPORT_ME_1=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
            MF_SAMPLE_LIB_1_NAME="My_Lib_Shared_1"
            MF_SAMPLE_LIB_1_EFFECTIVE_NAME="$<TARGET_FILE_NAME:My_Lib_Shared_1>"
            MF_SAMPLE_LIB_1_FOLDER="${CMAKE_CURRENT_BINARY_DIR}"
            MF_EXPORT_ME_1=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_1)
