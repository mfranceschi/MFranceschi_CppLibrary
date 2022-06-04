
add_library(My_Lib_Shared_5 SHARED lib.cpp)
target_link_libraries(My_Lib_Shared_5 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_5 PRIVATE MF_EXPORT_ME_5=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
        MF_SAMPLE_LIB_5_NAME="My_Lib_Shared_5"
        MF_SAMPLE_LIB_5_FOLDER="$<TARGET_FILE_DIR:My_Lib_Shared_5>"
        MF_EXPORT_ME_5=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_5)
