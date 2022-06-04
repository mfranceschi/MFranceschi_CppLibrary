
add_library(My_Lib_Shared_3 SHARED lib.hpp lib.cpp)
target_link_libraries(My_Lib_Shared_3 PRIVATE MF_SharedLibs)
target_compile_definitions(My_Lib_Shared_3 PRIVATE MF_EXPORT_ME_3=1)
target_compile_definitions(
        MF_SharedLibs_Tests
        PRIVATE
        MF_SAMPLE_LIB_3_NAME="My_Lib_Shared_3"
        MF_SAMPLE_LIB_3_FOLDER="$<TARGET_FILE_DIR:My_Lib_Shared_3>"
        MF_EXPORT_ME_3=0
)
add_dependencies(MF_SharedLibs_Tests My_Lib_Shared_3)
