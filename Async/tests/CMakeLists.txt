
add_executable(MF_Async_Tests)
target_link_libraries(
        MF_Async_Tests
        PRIVATE
            MF_Commons_Tests
            MF_Async
            MF_Strings
)

target_sources(
        MF_Async_Tests
        PRIVATE
            command_test.cpp
)

function(mf_add_test_exec Name)
    add_executable(TestExec_${Name} EXCLUDE_FROM_ALL executables/${Name}.cpp)
    add_compile_definitions(${Name}_Executable="$<TARGET_FILE:TestExec_${Name}>")
    add_dependencies(MF_Async_Tests TestExec_${Name})
endfunction()

mf_add_test_exec(OneForEachStream)
mf_add_test_exec(ReturnNbArgs)
mf_add_test_exec(LengthOfFirstArg)
mf_add_test_exec(LengthOfInput)
mf_add_test_exec(HelloWorld)
mf_add_test_exec(HelloWorldToStderr)
mf_add_test_exec(GenerateOutput)

set(LOREM_IPSUM_TWO_LINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/abc.txt)
if (NOT EXISTS ${LOREM_IPSUM_TWO_LINES_FILE})
    message(FATAL_ERROR "Could not find the lorem ipsum file!")
endif ()
target_compile_definitions(MF_Async_Tests PRIVATE LOREM_IPSUM_TWO_LINES_FILE="${LOREM_IPSUM_TWO_LINES_FILE}" CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}/")

gtest_discover_tests(MF_Async_Tests)
