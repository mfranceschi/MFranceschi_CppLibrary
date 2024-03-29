//
// Created by Utilisateur on 13/04/2022.
//

#if MF_UNIX
#    include <set>

#    include "MF/Environment.hpp"
#    include "MF/SharedLibs.hpp"
#    include "SharedLibs_DL_internals.hpp"

namespace MF
{
    namespace SharedLibs
    {
        static constexpr const char *ENV_VARIABLE_NAME = "LD_LIBRARY_PATH";
        static constexpr char ITEM_SEPARATOR = ':';

        static inline std::string readEnvVariable() {
            return Environment::getEnvOrDefault(ENV_VARIABLE_NAME, "");
        }

        static std::set<std::string> addedSearchPaths;

        void AddToSearchPaths(const std::string &path) {
            if (path.empty()) {
                throw std::invalid_argument("Input parameter 'path' is empty.");
            }

            if (addedSearchPaths.count(path) != 0) {
                // No-op if already present.
                return;
            }

            const std::string ldLibraryPath = readEnvVariable();
            std::string newLdLibraryPath =
                ldLibraryPath.empty() ? path
                                      : ldLibraryPath + std::to_string(ITEM_SEPARATOR) + path;
            Environment::setEnv(ENV_VARIABLE_NAME, newLdLibraryPath);

            addedSearchPaths.insert(path);
        }

        void RemoveFromSearchPaths(const std::string &path) {
            const auto iterator = addedSearchPaths.find(path);
            if (iterator == addedSearchPaths.cend()) {
                // No-op if missing.
                return;
            }

            const std::string ldLibraryPath = readEnvVariable();
            std::size_t pos = ldLibraryPath.find(ITEM_SEPARATOR);
            if (pos == std::string::npos) {
                if (ldLibraryPath == path) {
                    Environment::unsetEnv(ENV_VARIABLE_NAME);
                    return;
                }

                // Reaching this part is super weird but ok.
                return;
            }

            std::string resultingString;
            if (pos == 0) {
                resultingString = ldLibraryPath.substr(path.length() + 1);
            } else {
                resultingString =
                    ldLibraryPath.substr(0, pos - 1) + ldLibraryPath.substr(pos + path.length());
            }
            Environment::setEnv(ENV_VARIABLE_NAME, resultingString);

            addedSearchPaths.erase(iterator);
        }

        namespace internals
        {
            const std::set<std::string> &getSearchPaths() {
                return addedSearchPaths;
            }
        } // namespace internals
    } // namespace SharedLibs
} // namespace MF

#endif
