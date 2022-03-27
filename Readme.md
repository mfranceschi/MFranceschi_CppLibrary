# Martin Franceschi - C++ Library

This is a C++ library that I am creating in order to practice as many aspects as possible.

Particularly, I want to build a cross-platform library but by using Windows and Unix -specific functions. Every module
which is not marked as unfinished is then okay, tested on each of the following platforms and compilers: MSVC, MinGW,
Cygwin, WSL with gcc. MSVC and MinGW support both the UNICODE option.

It may not necessarily be useful to everyone, but at least I am learning a lot! ;)

## How to use it? With CMake!

TL;DR = add this subdirectory and link to the library `MF_ALL`.

You can also link to only the library that you need, each module is designed to only seek for the required dependencies.

Each module is made of a static library, a header file and a namespace. For example, to use the Filesystem library:

```cmake
add_subdirectory(MFranceschi_CppLibraries) # or use FetchContent
target_link_libraries(YourLib PRIVATE MF_Filesystem)
```

```cpp
#include "MF/Filesystem.hpp"

// ...
using namespace MF::Filesystem;

const Filesize_t sizeOfFile = GetFileSize("./Foo.txt");
const SFilename_t currentWorkingDirectory = GetCWD();
```

## Modules

- **Commons**
    - You should not need to include this yourself. It is just a few basic compile definitions.
- **Filesystem**
    - Several helper functions to help with files, directories.
    - My favourite tool is `ReadContents`: it reads an entire file in 1 pass and makes all its contents available in a
      byte array, in the fastest way possible.
- **Containers**
    - Among others, Java-like `Stream` interface.
- **Async**
    - Tools to help create processes and threads and monitor their execution.
- **System**
    - Adds low-level tools related to the OS.
- **Windows**
    - Some tools that are helpful when working with the Windows ecosystem.
    - It's non-breaking and empty when the OS is not Windows.
