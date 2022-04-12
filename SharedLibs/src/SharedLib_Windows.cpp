//
// Created by Utilisateur on 10/04/2022.
//

#if MF_WINDOWS
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
            SharedLib_Windows(const HMODULE &hmodule) : hmoduleHolder(hmodule) {
            }

            void *GetFunctionAsVoidPointer(const std::string &functionName) override {
                LOCK_t lock(mutex);

                FARPROC result = GetProcAddress(hmoduleHolder.get(), functionName.c_str());
                // "FARPROC" is a std::function<int()> as function pointer

                if (result == nullptr) {
                    throw element_not_found_exception(
                        "Unable to find given function: " + functionName);
                }

                return (void *)(result);
            }

            void *GetSystemItem() const noexcept override {
                return hmoduleHolder.get();
            }

           private:
            std::mutex mutex;
            using LOCK_t = std::lock_guard<std::mutex>;
            HmoduleCloser hmoduleHolder;
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
