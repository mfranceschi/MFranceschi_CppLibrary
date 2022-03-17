
add_executable(MF_Async_Tests)
target_link_libraries(
        MF_Async_Tests
        PRIVATE
            MF_Commons_Tests
            MF_Async
)

target_sources(
        MF_Async_Tests
        PRIVATE
            command_test.cpp
)

function(mf_add_test_exec Name)
    # set(Name_Target "TestExec_${Name}")
    add_executable(TestExec_${Name} EXCLUDE_FROM_ALL executables/${Name}.cpp)
    add_compile_definitions(${Name}_Executable="$<TARGET_FILE:TestExec_${Name}>")
    add_dependencies(MF_Async_Tests TestExec_${Name})
endfunction()

mf_add_test_exec(OneForEachStream)
mf_add_test_exec(ReturnNbArgs)
mf_add_test_exec(LengthOfFirstArg)
mf_add_test_exec(LengthOfInput)
mf_add_test_exec(HelloWorld)

include(GoogleTest)
gtest_discover_tests(MF_Async_Tests)
