
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
         * Throws std::invalid_argument if empty input.
         * No-op if the path is not a search path currently (as far as this function can know).
         */
        void RemoveFromSearchPaths(const std::string &path);

        class SharedLib;

        /**
         * Opens the given library by its name (relative or absolute path are allowed).
         * Throws an "element_not_found_exception" if the library could not be found.
         * If an error or exception is thrown when loading the library:
         * - an exception is thrown (Windows),
         * - a SIGABRT signal is raised (with the "dl" framework,
         *   it's a fixed behaviour of the "dlopen" function).
         */
        std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName);

        /**
         * Indicates if the provided library can be opened by the OpenExplicitly function.
         *
         * - It try-catches the OpenExplicitly call (Windows).
         * - It forks and calls OpenExplicitly, then closes ("dl" framework). Returns false if
         * "fork" failed. It returns whether that succeeded. No details provided in case of error.
         *
         */
        bool CanOpen(const std::string &libName) noexcept;

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
             * Throws an "element_not_found_exception" if the element is not found.
             *
             * Example: auto get_time_function =
             * myLib->GetFunction<decltype(&get_time)>("get_time");
             */
            template <typename FuncType>
            FuncType GetFunction(const std::string &functionName) {
                return reinterpret_cast<FuncType>(GetSymbolAsVoidPointer(functionName));
            }

            /**
             * Returns a reference to the variable named "name".
             * Throws an "element_not_found_exception" if the element is not found.
             *
             * Example: int& myNumber = myLib->GetVariable<int>("number");
             */
            template <typename VarType>
            VarType &GetVariable(const std::string &name) {
                return *(static_cast<VarType *>(GetSymbolAsVoidPointer(name)));
            }

            /**
             * Returns a void pointer to the symbol (variable or function) named "name".
             * Throws an "element_not_found_exception" if the symbol is not found.
             */
            void *GetPointer(const std::string &name) {
                return GetSymbolAsVoidPointer(name);
            }

            /**
             * Gets the underlying system item.
             * On Windows, use static_cast<HMODULE>(yourLib->GetSystemItem()).
             */
            virtual void *GetSystemItem() const noexcept = 0;

            /**
             * Gets the full path to the file of the shared library.
             * It uses '/' as separator.
             * Throws std::runtime_error if this info is not available.
             */
            virtual const std::string &GetSystemPath() = 0;

            //---------------------------------------------- Constructors - destructor

            // Deleted constructors
            SharedLib(const SharedLib &) = delete;
            SharedLib &operator=(const SharedLib &) = delete;
            SharedLib(SharedLib &&) = delete;
            SharedLib &operator=(SharedLib &&) = delete;

            virtual ~SharedLib() = default;

           protected:
            /**
             * Returns a pointer to the symbol designated by "name".
             * The return value is always a valid pointer that you can cast.
             * Throws an "element_not_found_exception" if if the element is not found.
             */
            virtual void *GetSymbolAsVoidPointer(const std::string &name) = 0;

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
