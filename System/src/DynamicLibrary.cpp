//
// Created by mfran on 04/09/2020.
//

#include <memory>

#if MF_WINDOWS

#    include "MF/LightWindows.hpp"
#    include "MF/Windows.hpp"

#else

#    include <dlfcn.h>

#    include <csignal>
#    include <cstring>

#endif

#include "MF/DynamicLibrary.hpp"

namespace MF
{
    namespace System
    {
#if MF_WINDOWS
        const char *DynamicLibrary::LocalExtension = ".dll";
#    define _GetLibraryPointer static_cast<HMODULE>(_library)
#else
        const char *DynamicLibrary::LocalExtension = ".so";
#endif

        void *DynamicLibrary::GetFunction(const std::string &functionName) {
            LOCK_t lockGuard(_mutex);

            void *functionAddress;

#if MF_WINDOWS
            functionAddress =
                reinterpret_cast<void *>(GetProcAddress(_GetLibraryPointer, functionName.c_str()));
            if (functionAddress == nullptr) {
                throw element_not_found_exception("Unable to find given function: " + functionName);
            }
#else
            (void)dlerror(); // Clear any previous error.
            functionAddress = dlsym(_library, functionName.c_str());
            if (dlerror()) {
                // We must run this check because the "functionAddress" value may be null but still
                // valid.
                throw element_not_found_exception("Unable to find given function: " + functionName);
            }
#endif
            return functionAddress;
        }

        void DynamicLibrary::AddToSearchPaths(const std::string &path) {
            if (path.size() < 2) {
                return;
            }

#if MF_WINDOWS
            auto wPath = Windows::ConvertString(path.c_str());
            AddDllDirectory(wPath.data());
#else
            char *ldLibraryPath = getenv("LD_LIBRARY_PATH");
            char *newLdLibraryPath = (char *)calloc(
                (ldLibraryPath ? strlen(ldLibraryPath) + 1 : 0) +
                    // Length of the environment variable, if any, + ":" separator
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
            LOCK_t lockGuard(_mutex);

            bool success;
#if MF_WINDOWS
            success = FreeLibrary(_GetLibraryPointer);
#else
            success = !dlclose(_library);
#endif

            if (success) {
                _library = nullptr;
            } else {
                // TODO handle error
            }
        }

        DynamicLibrary::DynamicLibrary(const std::string &libName) {
            LOCK_t lockGuard(_mutex);

#if MF_WINDOWS
            auto wLibName = Windows::ConvertString(libName.c_str());
            static constexpr HANDLE hFile = nullptr;
            static constexpr DWORD dwFlags = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
            _library = LoadLibraryExW(wLibName.data(), hFile, dwFlags);
#else
            _library = dlopen(libName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif

            if (_library != nullptr) {
                return; // We can add stuff there
            } else {
                throw element_not_found_exception(
                    "The requested library could not be found: " + libName);
            }
        }
    } // namespace System
} // namespace MF
