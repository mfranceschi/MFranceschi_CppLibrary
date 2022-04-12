//
// Created by Utilisateur on 13/04/2022.
//

#if MF_WINDOWS
#    include <map>

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

        class DllDirectoryCookieCloser
            : public ResourceCloser<DLL_DIRECTORY_COOKIE, decltype(&RemoveDllDirectory)> {
           public:
            DllDirectoryCookieCloser(DLL_DIRECTORY_COOKIE cookie)
                : ResourceCloser<DLL_DIRECTORY_COOKIE, decltype(&RemoveDllDirectory)>(
                      cookie, RemoveDllDirectory) {
            }
        };

        static std::map<std::string, DllDirectoryCookieCloser> addedSearchPaths;

        void AddToSearchPaths(const std::string &path) {
            if (path.empty()) {
                throw std::invalid_argument("Input parameter 'path' is empty.");
            }

            if (addedSearchPaths.count(path) != 0) {
                // No-op if already present.
                return;
            }

            auto wPath = Windows::ConvertString(path.c_str());
            DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(wPath.c_str());

            if (cookie == nullptr) {
                const auto lastError = GetLastError();
                throw std::runtime_error(
                    "An error occurred when trying to add '" + path + "' to search paths.");
            }

            addedSearchPaths.insert({path, cookie});
        }

        void RemoveFromSearchPaths(const std::string &path) {
            const auto iterator = addedSearchPaths.find(path);
            if (iterator == addedSearchPaths.cend()) {
                // No-op if missing.
                return;
            }

            // addedSearchPaths.erase(iterator); // TODO enable again later. Currently it causes SEH
            // exceptions.
        }

    } // namespace SharedLibs
} // namespace MF

#endif
