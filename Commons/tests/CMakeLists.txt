# Import the repo according to the pattern described here:
# https://github.com/CLIUtils/cmake/blob/master/AddGoogleTest.cmake

set(MF_GTEST_GIT_URL https://github.com/google/googletest)

find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)

    #cmake-lint: disable=readability/wonkycase
    FetchContent_Declare(
            googletest
            URL ${MF_GTEST_GIT_URL}/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

# Explanations on the below
# The static library "MF_Commons_Tests" contains GoogleTest headers
# and the "main" function (with setup items, etc.).
# Yep, it's a static library but it still has the main function. It is not forbidden, no? ^v^
# Other test executables can then simply link to this
# and won't need to have to write the main function themselves.

# To keep things separated, the tests executable of the "Commons" module is "MF_Commons_Tests_Tests"
# with the double suffix.

add_library(MF_Commons_Tests STATIC EXCLUDE_FROM_ALL)
target_sources(MF_Commons_Tests PRIVATE main_of_tests.cpp PUBLIC tests_data.hpp)
target_link_libraries(MF_Commons_Tests PUBLIC GTest::gtest MF_Commons)
target_include_directories(MF_Commons_Tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MF_Commons_Tests_Tests DummyFile.cpp)
target_link_libraries(MF_Commons_Tests_Tests MF_Commons_Tests)
gtest_discover_tests(MF_Commons_Tests_Tests)
