
//---------- Interface of class <DynamicLibrary> (file DynamicLibrary.hpp) ----------------
#if ! defined ( DYNAMIC_LIBRARY_H )
#define DYNAMIC_LIBRARY_H

//--------------------------------------------------------------- Includes
#include <exception>
#include <mutex>
#include <string>

//------------------------------------------------------------------ Types

// Role of DynamicLibrary:
// Wrapper for calls to a Dynamic Library.
// An instance holds a system handle for a
class DynamicLibrary
{
    //----------------------------------------------------------------- PUBLIC
public:
    using element_not_found_exception = std::invalid_argument;

    //------------------------------------------------------- Static utilities

    static const char* LocalExtension; // Has value ".dll" or ".so" depending on the OS.

    /**
     * Adds the given path to the list of paths which the system uses to find the library.
     * No-op if "path.length() < 2".
     * No return value nor error.
     */
    static void AddToSearchPaths(const std::string& path);

    //--------------------------------------------------------- Public methods

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
    template <typename FuncType>
    FuncType operator[](const std::string& functionName) {
        return reinterpret_cast<FuncType>(GetFunction(functionName));
    }

    /**
     * Gets the underlying system item.
     */
    inline void* GetSystemItem() const noexcept;

    //-------------------------------------------------- Operator overloadings

    // Comparison operators.
    inline bool operator== (const DynamicLibrary& b) const;
    inline bool operator!= (const DynamicLibrary& b) const;

    // Assignation (deleted)
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(DynamicLibrary&&) = delete;

//---------------------------------------------- Constructors - destructor

    // Unique constructor
    /**
     * Opens the given library by its name (relative or absolute path are allowed).
     * Throws an "element_not_found_exception" if the library could not be found.
     */
    explicit DynamicLibrary(const std::string& libName);

    // Deleted constructors
    DynamicLibrary() = delete;
    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary(DynamicLibrary&&) = delete;

    // Destructor. Frees the resource.
    virtual ~DynamicLibrary();

//---------------------------------------------------------------- PRIVATE

protected:
    using MUTEX_t = std::mutex;
    using LOCK_t = std::lock_guard<MUTEX_t>;

//------------------------------------------------------ Protected methods

//--------------------------------------------------- Protected attributes

    void* _library = nullptr; // Handle of the library. UNIX uses "void*", Win32 uses a HMODULE which is similar enough.
    MUTEX_t _mutex; // For thread-safety.

};

//------------------------------------------------------ Other definitions

inline bool DynamicLibrary::operator== (const DynamicLibrary& b) const {
    return _library == b._library;
}
inline bool DynamicLibrary::operator!= (const DynamicLibrary& b) const {
    return ! this->operator==(b);
}

inline void* DynamicLibrary::GetSystemItem() const noexcept {
    return _library;
}

#endif //DYNAMIC_LIBRARY_H
