
add_library(MF_Commons STATIC EXCLUDE_FROM_ALL src/dummy.cpp)
target_compile_definitions(
        MF_Commons
        PUBLIC
            MF_OS_IS_WINDOWS=$<BOOL:${WIN32}>
        )

add_subdirectory(tests)
