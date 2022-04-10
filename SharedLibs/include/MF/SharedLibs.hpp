
#if !defined(DYNAMIC_LIBRARY_H)
#    define DYNAMIC_LIBRARY_H

#    include <exception>
#    include <functional>
#    include <mutex>
#    include <string>

namespace MF
{
    namespace SharedLibs
    {
        /** @return ".dll" or ".so" depending on the OS. */
        constexpr const char *GetExtension();

        /**
         * Adds the given path to the list of paths which the system uses to find the library.
         * Throws std::invalid_argument if empty input.
         * No-op if the path is already a search path.
         */
        void AddToSearchPaths(const std::string &path);

        /**
         * Adds the given path to the list of paths which the system uses to find the library.
         * No-op if the path is not a search path currently (as far as this function can know).
         */
        void RemoveFromSearchPaths(const std::string &path);

        /** Wrapper for calls to a Dynamic Library.
         * An instance holds a valid system handle for one specific shared library object.
         */
        class SharedLib_OLD {
           public:
            using element_not_found_exception = std::invalid_argument;

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

            //---------------------------------------------- Constructors - destructor

            /**
             * Unique constructor.
             * Opens the given library by its name (relative or absolute path are allowed).
             * Throws an "element_not_found_exception" if the library could not be found.
             */
            explicit SharedLib_OLD(const std::string &libName);

            // Deleted constructors
            SharedLib_OLD() = delete;
            SharedLib_OLD(const SharedLib_OLD &) = delete;
            SharedLib_OLD &operator=(const SharedLib_OLD &) = delete;
            SharedLib_OLD(SharedLib_OLD &&) = delete;
            SharedLib_OLD &operator=(SharedLib_OLD &&) = delete;

            virtual ~SharedLib_OLD();

           protected:
            using MUTEX_t = std::mutex;
            using LOCK_t = std::lock_guard<MUTEX_t>;

            void *libHandle = nullptr; // Handle of the library. UNIX uses "void*", Win32 uses a
                                       // HMODULE which is similar enough.
            MUTEX_t mutex; // For thread-safety.
        };

        class SharedLib;

        /**
         * Opens the given library by its name (relative or absolute path are allowed).
         * Throws an "element_not_found_exception" if the library could not be found.
         */
        std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName);

        /**
         * Wrapper for calls to a Dynamic Library.
         * An instance holds a valid system handle for one specific shared library object.
         */
        class SharedLib {
           public:
            using element_not_found_exception = std::invalid_argument;

            /**
             * Returns a function pointer to the procedure requested in "functionName".
             * The return value is always a valid function pointer that you can cast.
             * Throws a "library_not_opened_exception" if not "IsOpen()".
             * Throws an "element_not_found_exception" if it failed because "functionName" does not
             * refer to something valid in the DL.
             */
            virtual void *GetFunction(const std::string &functionName) = 0;

            /**
             * Casts the result of "GetFunction".
             */
            template <typename FuncType>
            FuncType operator[](const std::string &functionName) {
                return reinterpret_cast<FuncType>(GetFunction(functionName));
            }

            /**
             * Gets the underlying system item.
             * On Windows, use static_cast<HMODULE>.
             */
            virtual void *GetSystemItem() const noexcept = 0;

            //---------------------------------------------- Constructors - destructor

            // Deleted constructors
            SharedLib(const SharedLib_OLD &) = delete;
            SharedLib &operator=(const SharedLib_OLD &) = delete;
            SharedLib(SharedLib_OLD &&) = delete;
            SharedLib &operator=(SharedLib_OLD &&) = delete;

            virtual ~SharedLib() = default;

           protected:
            SharedLib() = default;

            friend std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName);
        };

        inline void *SharedLib_OLD::GetSystemItem() const noexcept {
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
