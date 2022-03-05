
add_library(MF_Commons STATIC EXCLUDE_FROM_ALL)
target_compile_definitions(
        MF_Commons
        PUBLIC
        MF_OS_IS_WINDOWS=$<BOOL:${WIN32}>
)
target_sources(MF_Commons PRIVATE src/dummy.cpp)

add_subdirectory(tests)
