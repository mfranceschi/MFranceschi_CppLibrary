
add_executable(GetYesOrNo GetYesOrNo.cpp)
target_link_libraries(GetYesOrNo MF_Console)

find_package(Python3)

add_test(
        NAME GetYesOrNo_PyTests
        COMMAND
            Python3::Interpreter
            ${CMAKE_CURRENT_SOURCE_DIR}/GetYesOrNo_tests.py
            $<TARGET_FILE:GetYesOrNo>
)
set_tests_properties(GetYesOrNo_PyTests PROPERTIES TIMEOUT 5)

#[[
add_executable(MF_Console_Tests)
target_link_libraries(
        MF_Console_Tests
        PRIVATE
        MF_Console
        MF_Commons_Tests
)

target_sources(
        MF_Console_Tests
        PRIVATE
        ctime_tests.cpp
)

gtest_discover_tests(MF_Console_Tests)
]]
