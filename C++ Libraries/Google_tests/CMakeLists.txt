# "Google_test" is the project name
project (Google_test)

# Import the repo according to the pattern described here:
# https://stackoverflow.com/a/38026254/11996851.
include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
ExternalProject_Add(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

# "Google_Tests_run" is the target name
set(MY_SOURCES ../Date.cpp ../File.cpp ../Toolbox.cpp)
add_executable(Google_Tests_run file_tests.cpp date_tests.cpp ${MY_SOURCES})
add_dependencies(Google_Tests_run googletest)
add_compile_definitions(UNICODE)
target_link_libraries(Google_Tests_run gtest gtest_main)
