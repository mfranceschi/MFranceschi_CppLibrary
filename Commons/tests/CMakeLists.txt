add_library(MF_Commons_Tests STATIC EXCLUDE_FROM_ALL)
target_sources(MF_Commons_Tests PRIVATE main_of_tests.cpp PUBLIC tests_data.hpp)
target_link_libraries(MF_Commons_Tests PUBLIC GTest::gtest gmock MF_Commons)
target_include_directories(MF_Commons_Tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(
            MF_Commons_Tests
            PRIVATE
            MF_LOOK_FOR_MEMORY_LEAKS=1
            PUBLIC
            _CRTDBG_MAP_ALLOC=1
            _DEBUG=1
    )
endif()

add_executable(MF_Commons_Tests_Tests DummyFile.cpp)
target_link_libraries(MF_Commons_Tests_Tests MF_Commons_Tests)
gtest_discover_tests(MF_Commons_Tests_Tests)
