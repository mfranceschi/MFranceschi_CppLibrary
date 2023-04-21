
include(TestBigEndian)
test_big_endian(MF_BIG_ENDIAN)

add_library(MF_Commons STATIC EXCLUDE_FROM_ALL)
target_compile_definitions(
        MF_Commons
        PUBLIC

        MF_APPLE=$<BOOL:${APPLE}>
        MF_UNIX=$<BOOL:${UNIX}>
        MF_WINDOWS=$<BOOL:${WIN32}>

        MF_CYGWIN=$<BOOL:${CYGWIN}>
        MF_MINGW=$<BOOL:${MINGW}>
        MF_GCC=$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>

        MF_BIG_ENDIAN=${MF_BIG_ENDIAN}
        MF_LITTLE_ENDIAN=$<NOT:${MF_BIG_ENDIAN}>
)
target_sources(MF_Commons
        PUBLIC include/MF/Commons.hpp
        PRIVATE src/dummy.cpp
        )
target_include_directories(MF_Commons PUBLIC include)

if (MF_IN_DEV)
    target_compile_definitions(MF_Commons PUBLIC _CRTDBG_MAP_ALLOC=1)
    add_subdirectory(tests)
endif ()
