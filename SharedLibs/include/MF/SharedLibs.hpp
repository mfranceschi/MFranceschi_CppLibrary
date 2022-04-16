
#if !defined(DYNAMIC_LIBRARY_H)
#    define DYNAMIC_LIBRARY_H

#    include <exception>
#    include <functional>
#    include <memory>
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
             * Returns a function pointer to the procedure designated by "functionName".
             * The return value is always a valid function pointer that you can cast.
             * Throws an "element_not_found_exception" if if the element is not found.
             *
             * Example: auto get_time_function =
             * myLib->GetFunction<decltype(&get_time)>("get_time");
             */
            template <typename FuncType>
            FuncType GetFunction(const std::string &functionName) {
                return reinterpret_cast<FuncType>(GetFunctionAsVoidPointer(functionName));
            }

            /**
             * Gets the underlying system item.
             * On Windows, use static_cast<HMODULE>.
             */
            virtual void *GetSystemItem() const noexcept = 0;

            //---------------------------------------------- Constructors - destructor

            // Deleted constructors
            SharedLib(const SharedLib &) = delete;
            SharedLib &operator=(const SharedLib &) = delete;
            SharedLib(SharedLib &&) = delete;
            SharedLib &operator=(SharedLib &&) = delete;

            virtual ~SharedLib() = default;

           protected:
            /**
             * Returns a function pointer to the procedure designated by "functionName".
             * The return value is always a valid function pointer that you can cast.
             * Throws an "element_not_found_exception" if if the element is not found.
             */
            virtual void *GetFunctionAsVoidPointer(const std::string &functionName) = 0;

            SharedLib() = default;
        };

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
