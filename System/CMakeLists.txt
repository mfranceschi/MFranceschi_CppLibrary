
add_library(MF_System STATIC)

target_sources(MF_System
        PRIVATE
            src/Date.cpp
            src/DateUtils.cpp
            src/DateUtils_MaxYear.cpp
            src/DynamicLibrary.cpp
        PUBLIC
            # include/MF/Command.hpp
            include/MF/Date.hpp
            include/MF/DateError.hpp
            include/MF/DateTypes.hpp
            include/MF/DateUtils.hpp
            include/MF/DynamicLibrary.hpp
)
target_link_libraries(MF_System PUBLIC MF_Commons MF_Windows)
target_include_directories(MF_System PUBLIC include)

add_subdirectory(tests)
