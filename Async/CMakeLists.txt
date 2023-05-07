
add_library(MF_Async STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Async PUBLIC include)
target_link_libraries(MF_Async PUBLIC MF_Commons MF_Filesystem PRIVATE MF_Windows_Header MF_SystemErrors MF_Windows)
target_sources(
        MF_Async
        PRIVATE
        src/Command_commons.hpp
        src/Command_stateful_base.cpp
        src/Command_run.cpp
        src/Command_commons_windows.hpp
        src/Command_console_choices_windows.cpp
        src/Command_commons_windows.cpp
        src/Command_runner_windows.cpp
            #[[
            src/Command.cpp
            src/CommandHelper.hpp
            src/CommandHelper_Generic.cpp
            src/CommandHelper_Unix.cpp
            src/CommandHelper_Windows.cpp
            # src/UnixAPIHelper.cpp
            # src/UnixAPIHelper.hpp
            src/WindowsAPIHelper.cpp
            src/WindowsAPIHelper.hpp
        PUBLIC
            include/MF/Command.hpp
            include/MF/Timer.hpp
            ]]
)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
