//
// Created by Utilisateur on 13/04/2022.
//

#if MF_UNIX
#    ifndef _GNU_SOURCE
#        define _GNU_SOURCE 1
#    endif
#    include <dlfcn.h>
#    include <link.h>

#    include <mutex>

#    include "MF/SharedLibs.hpp"
#    include "SharedLibs_DL_internals.hpp"

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
            ResourceType resource;
            const Closer closer;
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

            void *GetSymbolAsVoidPointer(const std::string &functionName) override {
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

            virtual const std::string &GetSystemPath() override {
                LOCK_t lock(mutex);

                if (systemPath.empty()) {
                    link_map *details = nullptr;
                    if (dlinfo(libCloser.get(), RTLD_DI_LINKMAP, &details) == 0) {
                        systemPath = details->l_name;
                    } else {
                        throw std::runtime_error("Failed to get the system path.");
                    }
                }

                return systemPath;
            }

           private:
            std::mutex mutex;
            using LOCK_t = std::lock_guard<std::mutex>;
            DlOpenResultCloser libCloser;

            std::string systemPath;
        };

        std::shared_ptr<SharedLib> OpenExplicitly(const std::string &libName) {
            std::string fixedName = libName;
            static const std::string extension = GetExtension();
            if (fixedName.size() > extension.size() &&
                fixedName.substr(fixedName.size() - extension.size()) != extension) {
                fixedName += extension;
            }

            DlOpenResult_t libHandle = nullptr;

            for (const std::string &namePrefix : {"lib", ""}) {
                const std::string nameToTry = namePrefix + fixedName;

                libHandle = dlopen(fixedName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
                if (libHandle != nullptr) {
                    return std::make_shared<SharedLib_Dl>(libHandle);
                }

                /*
                 * The following is a brute-force approach.
                 * We test the "nameToTry" by prepending each added search path.
                 */
                for (const std::string &addedSearchPath : internals::getSearchPaths()) {
                    const std::string nameWithPathToTry = *(--addedSearchPath.cend()) == '/'
                                                              ? addedSearchPath + nameToTry
                                                              : addedSearchPath + "/" + nameToTry;

                    libHandle = dlopen(nameWithPathToTry.c_str(), RTLD_LAZY | RTLD_GLOBAL);
                    if (libHandle != nullptr) {
                        return std::make_shared<SharedLib_Dl>(libHandle);
                    }
                }
            }

            throw SharedLib::element_not_found_exception(
                "Could not open library with name " + fixedName);
        }
    } // namespace SharedLibs
} // namespace MF

#endif
