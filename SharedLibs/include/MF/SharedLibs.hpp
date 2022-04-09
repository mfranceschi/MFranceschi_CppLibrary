
//---------- Interface of class <SharedLib> (file SharedLib.hpp) ----------------
#if !defined(DYNAMIC_LIBRARY_H)
#    define DYNAMIC_LIBRARY_H

//--------------------------------------------------------------- Includes
#    include <exception>
#    include <functional>
#    include <mutex>
#    include <string>

//------------------------------------------------------------------ Types

namespace MF
{
    namespace SharedLibs
    {
        /** @return ".dll" or ".so" depending on the OS. */
        constexpr const char *GetExtension();

        /**
         * Adds the given path to the list of paths which the system uses to find the library.
         * No-op if "path.length() < 2".
         * No return value nor error.
         */
        void AddToSearchPaths(const std::string &path);

        // Role of SharedLib:
        // Wrapper for calls to a Dynamic Library.
        // An instance holds a valid system handle for one specific shared library object.
        class SharedLib {
            //----------------------------------------------------------------- PUBLIC
           public:
            using element_not_found_exception = std::invalid_argument;

            //------------------------------------------------------- Static utilities

            //--------------------------------------------------------- Public methods

            /**
             * Returns a function pointer to the procedure requested in "functionName".
             * The return value is always a valid function pointer that you can cast.
             * Throws a "library_not_opened_exception" if not "IsOpen()".
             * Throws an "element_not_found_exception" if it failed because "functionName" does not
             * refer to something valid in the DL.
             */
            void *GetFunction(const std::string &functionName);

            /**
             * Casts the result of "GetFunction".
             */
            template <typename FuncType>
            FuncType operator[](const std::string &functionName) {
                return reinterpret_cast<FuncType>(GetFunction(functionName));
            }

            /**
             * Gets the underlying system item.
             */
            inline void *GetSystemItem() const noexcept;

            //-------------------------------------------------- Operator overloadings

            // Assignation (deleted)
            SharedLib &operator=(const SharedLib &) = delete;

            SharedLib &operator=(SharedLib &&) = delete;

            //---------------------------------------------- Constructors - destructor

            // Unique constructor
            /**
             * Opens the given library by its name (relative or absolute path are allowed).
             * Throws an "element_not_found_exception" if the library could not be found.
             */
            explicit SharedLib(const std::string &libName);

            // Deleted constructors
            SharedLib() = delete;

            SharedLib(const SharedLib &) = delete;

            SharedLib(SharedLib &&) = delete;

            // Destructor. Frees the resource.
            virtual ~SharedLib();

            //---------------------------------------------------------------- PRIVATE

           protected:
            using MUTEX_t = std::mutex;
            using LOCK_t = std::lock_guard<MUTEX_t>;

            //------------------------------------------------------ Protected methods

            //--------------------------------------------------- Protected attributes

            void *libHandle = nullptr; // Handle of the library. UNIX uses "void*", Win32 uses a
                                       // HMODULE which is similar enough.
            MUTEX_t mutex; // For thread-safety.
        };

        //------------------------------------------------------ Other definitions

        inline void *SharedLib::GetSystemItem() const noexcept {
            return libHandle;
        }

        constexpr const char *GetExtension() {
#    if MF_WINDOWS
            return ".dll";
#    else
            return ".so";
#    endif
        }
    } // namespace SharedLibs

} // namespace MF

#endif // DYNAMIC_LIBRARY_H
