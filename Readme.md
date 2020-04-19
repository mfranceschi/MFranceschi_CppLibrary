# Martin Franceschi - C++ Library

This is a C++ library that I am creating in order to practise as many aspects as possible. 

Particularly, I want to build a cross-platform library but by using Windows and Unix -specific functions. 

It may not necessary be useful to everyone, but at least I am learning a lot! ;)

## How to use it?

Normally, you can simply link this library to your project using CMake, by linking your target to the library `MFranceschi_CppLibrary`. 

The code should be quite documented, and you can find extra help by visiting this project's Wiki on Github. 

## Unfinished works
- {`C++`} - The following libraries:
  - __"File" module__: gives some useful tools for files from their filename, and tools for making ifstreams manipulation easier. All functions are meant to be cross-platforms.
  - __"Date" class__: extends the `struct tm` from the C standard library with milliseconds, and overloads some operators such as comparison, +, -, etc.
  - __"CSV-Parser" and "QuickCSVParser" classes__: the 1st class encapsulates an ifstream specifically for parsing CSV files, the other one encapsulates a CSV-Parser and offers a single-command possibility of retrieving an entire file.
  - __"GCoord" struct__: a simple class having two attributes: *latitude* and *longitude* and some associated functionnalities such as calculus.
