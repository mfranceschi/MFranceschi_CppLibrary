
add_library(MF_Async STATIC EXCLUDE_FROM_ALL)
target_include_directories(MF_Async PUBLIC include)
target_link_libraries(MF_Async PUBLIC MF_Commons MF_Filesystem)
target_sources(
        MF_Async
        PRIVATE
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
)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
