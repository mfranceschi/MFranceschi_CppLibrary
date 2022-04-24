//
// Created by Utilisateur on 10/04/2022.
//

#if MF_WINDOWS
#    include <vector>

#    include "MF/LightWindows.hpp"
#    include "MF/SharedLibs.hpp"

namespace MF
{
    namespace SharedLibs
    {
        template <typename ResourceType, typename Closer>
        class ResourceCloser {
           public:
            ResourceCloser(ResourceType resource, Closer closer)
                : resource(resource), closer(closer) {
            }

            const ResourceType &get() const {
                return resource;
            }

            virtual ~ResourceCloser() {
                closer(resource);
            }

           private:
            const Closer closer;
            ResourceType resource;
        };

        class HmoduleCloser : public ResourceCloser<HMODULE, decltype(&FreeLibrary)> {
           public:
            HmoduleCloser(HMODULE hmodule)
                : ResourceCloser<HMODULE, decltype(&FreeLibrary)>(hmodule, FreeLibrary) {
            }
        };

        class SharedLib_Windows : public SharedLib {
           public:
            SharedLib_Windows(const HMODULE &hmodule) : libCloser(hmodule) {
            }

            void *GetSymbolAsVoidPointer(const std::string &name) override {
                LOCK_t lock(mutex);

                FARPROC result = GetProcAddress(libCloser.get(), name.c_str());
                // "FARPROC" is a function pointer of type int(void).

                if (result == nullptr) {
                    throw element_not_found_exception("Unable to find given symbol: " + name);
                }

                return (void *)(result);
            }

            void *GetSystemItem() const noexcept override {
                return libCloser.get();
            }

            std::string GetSystemPath() const override {
                static constexpr std::size_t MODULE_PATH_LENGTH = 1e6;
                std::vector<char> modulePath(MODULE_PATH_LENGTH);
                GetModuleFileNameA(libCloser.get(), modulePath.data(), MODULE_PATH_LENGTH);
                return modulePath.data();
            }

           private:
            std::mutex mutex;
            using LOCK_t = std::lock_guard<std::mutex>;
            HmoduleCloser libCloser;
        };

        std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName) {
            static constexpr HANDLE hFile = nullptr;
            static constexpr DWORD dwFlags = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;

            HMODULE libHandle = LoadLibraryExA(libName.data(), hFile, dwFlags);
            if (libHandle == nullptr) {
                throw SharedLib::element_not_found_exception(
                    "Could not open library with name " + libName);
            }

            return std::make_shared<SharedLib_Windows>(libHandle);
        }
    } // namespace SharedLibs
} // namespace MF

#endif
