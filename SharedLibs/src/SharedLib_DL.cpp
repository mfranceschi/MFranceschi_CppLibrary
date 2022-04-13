//
// Created by Utilisateur on 13/04/2022.
//

//
// Created by Utilisateur on 10/04/2022.
//

#if MF_UNIX
#    include <dlfcn.h>

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

        using DlOpenResult_t = void *;

        class DlOpenResultCloser : public ResourceCloser<DlOpenResult_t, decltype(&dlclose)> {
           public:
            DlOpenResultCloser(DlOpenResult_t value)
                : ResourceCloser<DlOpenResult_t, decltype(&dlclose)>(value, dlclose) {
            }
        };

        class SharedLib_Dl : public SharedLib {
           public:
            SharedLib_Dl(const DlOpenResult_t &handle) : libCloser(handle) {
            }

            void *GetFunctionAsVoidPointer(const std::string &functionName) override {
                LOCK_t lock(mutex);

                (void)dlerror(); // Clear any previous error.
                void *result = dlsym(libCloser.get(), functionName.c_str());
                if (dlerror()) {
                    // We must run this check because the "functionAddress" value may be null but
                    // still valid.
                    throw element_not_found_exception(
                        "Unable to find given function: " + functionName);
                }

                return result;
            }

            void *GetSystemItem() const noexcept override {
                return libCloser.get();
            }

           private:
            std::mutex mutex;
            using LOCK_t = std::lock_guard<std::mutex>;
            DlOpenResultCloser libCloser;
        };

        std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName) {
            DlOpenResult_t libHandle = dlopen(libName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
            if (libHandle == nullptr) {
                throw SharedLib::element_not_found_exception(
                    "Could not open library with name " + libName);
            }

            return std::make_shared<SharedLib_Dl>(libHandle);
        }
    } // namespace SharedLibs
} // namespace MF

#endif
