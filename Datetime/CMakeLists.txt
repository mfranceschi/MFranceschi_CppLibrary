
add_library(MF_Datetime STATIC)

target_sources(MF_Datetime
        PRIVATE
            src/Date.cpp
            src/DateUtils.cpp
            src/DateUtils_MaxYear.cpp
        PUBLIC
            include/MF/Datetime.hpp
)
target_link_libraries(MF_Datetime PUBLIC MF_Commons MF_Windows)
target_include_directories(MF_Datetime PUBLIC include)

if(MF_IN_DEV)
    add_subdirectory(tests)
endif()
