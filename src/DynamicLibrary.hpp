//
// Created by mfran on 04/09/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DYNAMICLIBRARY_HPP
#define MFRANCESCHI_CPPLIBRARIES_DYNAMICLIBRARY_HPP

#include <exception>
#include <mutex>
#include <string>

class DynamicLibrary {
public:
    using element_not_found_exception = std::invalid_argument;
    using library_not_opened_exception = std::logic_error;
    using library_opened_exception = std::logic_error;

    /** Has value ".dll" or ".so" depending on the OS. */
    static const char* LocalExtension;

    /**
     * Opens the given library by its name (relative or absolute path are allowed).
     * Throws a "library_opened_exception" if "IsOpen()".
     * Throws an "element_not_found_exception" if the library could not be found.
     */
    void Open(const std::string& libName);

    /**
     * Returns a function pointer to the procedure requested in "functionName".
     * The return value is always a valid function pointer that you can cast.
     * Throws a "library_not_opened_exception" if not "IsOpen()".
     * Throws an "element_not_found_exception" if it failed because "functionName" does not refer to something valid in the DL.
     */
    void* GetFunction(const std::string& functionName);

    /**
     * Casts the result of "GetFunction".
     */
    template <typename R, typename ... A>
    R (*GetFunctionT(const std::string& functionName)) (A...) {
        return static_cast<R(*)(A...)>(GetFunction(functionName));
    }

    /**
     * Returns true if the current instance is associated with a currently opened library.
     */
    bool IsOpen() const noexcept { return _library; }

    /**
     * Releases all resources used by the library.
     * Throws a "library_not_opened_exception" if not "IsOpen()".
     */
    void Free();

    /**
     * Adds the given path to the list of paths which the system uses to find the library.
     * No-op if "path.length() < 2".
     * No return value nor error.
     */
    static void AddToSearchPaths(const std::string& path);

    /**
     * Creates an empty instance.
     */
    DynamicLibrary() = default;

    /**
     * Creates an instance with "libName" sent to "Open(libName)". Same exceptions...
     */
    explicit DynamicLibrary(const std::string& libName);

    /**
     * Calls "Free()" if necessary.
     */
    ~DynamicLibrary();

protected:
    using _mutex_t = std::mutex;
    using _LOCK_t = std::lock_guard<_mutex_t>;
    void throwIfNotOpen() const; // Helper function.

    void* _library = nullptr; // Handle of the library. UNIX uses "void*", Win32 uses a HMODULE which is similar enough.
    _mutex_t _mutex; // For thread-safety.
};


#endif //MFRANCESCHI_CPPLIBRARIES_DYNAMICLIBRARY_HPP
