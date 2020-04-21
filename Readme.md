# Martin Franceschi - C++ Library

This is a C++ library that I am creating in order to practice as many aspects as possible.

Particularly, I want to build a cross-platform library but by using Windows and Unix -specific functions. Every module which is not marked as unfinished is then okay, tested on each of the following platforms and compilers: MSVC, MinGW, Cygwin, WSL with gcc. MSVC and MinGW support both the UNICODE option.

It may not necessary be useful to everyone, but at least I am learning a lot! ;)

## How to use it?

The code should be quite documented, and you can find extra help by visiting **this project's Wiki on Github**.

Normally, you can simply link this library to your project using CMake, by linking your target to the library named by the variable `MF_Lib_Libname`. For example, you can do:
```cmake
add_subdirectory(MFranceschi_CppLibrary)
target_link_libraries(YourProjectName PUBLIC ${MF_Lib_Libname})
```

To include stuff, just use the header file inside the `includes/` directory.

## Modules
- __"Command"__: **NOT FINISHED YET** highly-customizable shell command executor, and many more (PowerShell script call, output redirections, call a function when script is over...). Some settings require threads, if threads are not available then these functions are without effect.
- __"CSVParser"__: **NOT FINISHED YET** simple tool to get a whole CSV file into a standard library container.
- __"Date"__: **NOT FINISHED YET** extends the `struct tm` from the C standard library with milliseconds, and overloads some operators such as comparison, +, -, etc.
- __"File"__: gives some useful tools for files from their filename, and tools for making `ifstream` manipulation easier. All functions are meant to be cross-platforms.
- __"GeoCoord"__: **NOT FINISHED YET** a simple class having two attributes: *latitude* and *longitude* and some associated tools such as calculus.
- __"Toolbox"__: helper functions which I decided to make available, even to the user.
