
add_library(MF_Commons STATIC EXCLUDE_FROM_ALL)
target_compile_definitions(
        MF_Commons
        PUBLIC

        MF_APPLE=$<BOOL:${APPLE}>
        MF_UNIX=$<BOOL:${UNIX}>
        MF_WINDOWS=$<BOOL:${WIN32}>

        MF_CYGWIN=$<BOOL:${CYGWIN}>
        MF_MINGW=$<BOOL:${MINGW}>
)
target_sources(MF_Commons PRIVATE src/dummy.cpp)

add_subdirectory(tests)
