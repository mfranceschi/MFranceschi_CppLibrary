//
// Created by Utilisateur on 10/04/2022.
//

#if MF_WINDOWS
#    include <algorithm>
#    include <vector>

#    include "MF/LightWindows.hpp"
#    include "MF/SharedLibs.hpp"
#    include "MF/Windows.hpp"

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

            const std::string &GetSystemPath() override {
                LOCK_t lock(mutex);
                if (!systemPath.empty()) {
                    return systemPath;
                }

                std::vector<char> modulePath(MAX_PATH);
                if (GetModuleFileNameA(libCloser.get(), modulePath.data(), MAX_PATH) == 0) {
                    throw MF::Windows::GetCurrentSystemError();
                }
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                    modulePath.clear();

                    // We use the value of 32767 as stated in the docs:
                    // https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
                    modulePath.reserve(UINT16_MAX);
                    if (GetModuleFileNameA(libCloser.get(), modulePath.data(), UINT16_MAX) == 0) {
                        throw MF::Windows::GetCurrentSystemError();
                    }
                }

                std::string result = modulePath.data();
                std::replace(result.begin(), result.end(), '\\', '/');
                auto iterator = result.rfind(".DLL");
                result.replace(iterator, 4, GetExtension());

                systemPath = result;

                return systemPath;
            }

           private:
            std::mutex mutex;
            using LOCK_t = std::lock_guard<std::mutex>;
            HmoduleCloser libCloser;

            std::string systemPath;
        };

        std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName) {
            HMODULE libHandle =
                LoadLibraryExA(libName.data(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
            if (libHandle == nullptr) {
                throw SharedLib::element_not_found_exception(
                    "Could not open library with name " + libName);
            }

            return std::make_shared<SharedLib_Windows>(libHandle);
        }
    } // namespace SharedLibs
} // namespace MF

#endif
