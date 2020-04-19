add_executable(TimingExperience TimingExperience.hpp TimingExperience.cpp)
target_link_libraries(TimingExperience PRIVATE ${MF_Lib_Libname} "Shlwapi.dll")

if (MINGW)
    set_source_files_properties(TimingExperience.cpp PROPERTIES COMPILE_FLAGS "-Wno-conversion-null")
endif()
