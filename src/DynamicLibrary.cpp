//
// Created by mfran on 04/09/2020.
//

#include <memory>
#if defined(_WIN32)
#   include <Windows.h>
#   include "WindowsAPIHelper.hpp"
#else
#   include <dlfcn.h>
#   include <cstring>
#   include <signal.h>
#endif

#include "DynamicLibrary.hpp"

#if defined(_WIN32)
const char* DynamicLibrary::LocalExtension = ".dll";
#define _GetLibraryPointer static_cast<HMODULE>(_library)
#else
const char* DynamicLibrary::LocalExtension = ".so";
#endif

void DynamicLibrary::Open(const std::string& libName) {
    _LOCK_t lockGuard(_mutex);
    if (IsOpen()) {
        throw library_opened_exception ("The current instance is already opened.");
    }

#if defined(_WIN32)
    std::unique_ptr<const wchar_t[]> wLibName(Windows_ConvertString(libName.c_str()));
    HANDLE hFile = nullptr;
    DWORD dwFlags = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
    _library = LoadLibraryExW(wLibName.get(), hFile, dwFlags);
#else
    library = dlopen(libName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif

    if (IsOpen()) {
        return; // We can add stuff there
    }
    else {
        throw element_not_found_exception ("The requested library could not be found: " + libName);
    }
}

void* DynamicLibrary::GetFunction(const std::string& functionName) {
    _LOCK_t lockGuard(_mutex);
    throwIfNotOpen();

    void* functionAddress;

#if defined(_WIN32)
    functionAddress = static_cast<void*>(GetProcAddress(_GetLibraryPointer, functionName.c_str()));
    if (functionAddress == nullptr) {
        throw element_not_found_exception ("Unable to find given function: " + functionName);
    }
#else
    (void)dlerror(); // Clear any previous error.
    functionAddress = dlsym(library, functionName.c_str());
    if (dlerror()) {
        throw element_not_found_exception ("Unable to find given function: " + functionName);
    }
#endif
    return functionAddress;
}

void DynamicLibrary::Free() {
    _LOCK_t lockGuard(_mutex);
    throwIfNotOpen();

    bool success;
#if defined(_WIN32)
    success = FreeLibrary(_GetLibraryPointer);
#else
    success = !dlclose(library);
#endif

    if (success) {
        _library = nullptr;
    }
    else {
        // TODO handle error
    }
}

void DynamicLibrary::AddToSearchPaths(const std::string& path) {
    if (path.size() < 2) {
        return;
    }

#if defined(_WIN32)
    std::unique_ptr<const wchar_t[]> wPath(Windows_ConvertString(path.c_str()));
    AddDllDirectory(wPath.get());
#else
    char* ldLibraryPath = getenv("LD_LIBRARY_PATH");
    char* newLdLibraryPath = (char*) calloc(
        (ldLibraryPath ? strlen(ldLibraryPath) + 1 : 0) + // Length of the environment variable, if any, + ":" separator
        path.length() + // Length of the value to add
        1, // End of string
        1);
    strcpy(newLdLibraryPath, ldLibraryPath);
    strcat(newLdLibraryPath, ":");
    strcat(newLdLibraryPath, path.c_str());
    setenv("LD_LIBRARY_PATH", newLdLibraryPath, true);
#endif
}

DynamicLibrary::~DynamicLibrary() {
    if (IsOpen()) {
        Free();
    }
}

void DynamicLibrary::throwIfNotOpen() const {
    if (!IsOpen()) {
        throw library_not_opened_exception("No library opened in current instance.");
    }
}

DynamicLibrary::DynamicLibrary(const std::string &libName) : DynamicLibrary() {
    Open(libName);
}
