
add_library(MF_SystemErrors STATIC)

file(GLOB Private_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB Public_Sources
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MF/*.hpp)

target_sources(MF_SystemErrors
        PRIVATE
        ${Private_Sources}
        PUBLIC
        ${Public_Sources}
        )
target_link_libraries(MF_SystemErrors PUBLIC MF_Commons MF_Windows)
target_include_directories(MF_SystemErrors PUBLIC include)

include(CheckStrerrorFunctionsExist.cmake)

target_compile_definitions(MF_SystemErrors
        PUBLIC
        MF_SystemErrors_WSA=$<OR:$<BOOL:${MF_SystemErrors_WSA}>,$<BOOL:${MF_IN_DEV}>>
        PRIVATE
        MF_HAS_strerrordesc_np=$<BOOL:${MF_HAS_strerrordesc_np}>
        MF_HAS_strerror_r=$<BOOL:${MF_HAS_strerror_r}>
        MF_HAS_strerror_s=$<BOOL:${MF_HAS_strerror_s}>
        MF_HAS_strerror_l=$<BOOL:${MF_HAS_strerror_l}>
        __STDC_WANT_LIB_EXT1__=1
        )

if (MF_IN_DEV)
    add_subdirectory(tests)
endif ()
