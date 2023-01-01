//
// Created by MartinF on 25/06/2022.
//

#if MF_UNIX
#    include <unistd.h>

#    include <cstdlib>
#    include <cstring>
#    include <functional>
#    include <stdexcept>

#    include "MF/Environment.hpp"

namespace MF
{
    namespace Environment
    {
        static void assertNameIsNotEmpty(const std::string& name) {
            if (name.empty()) {
                throw std::runtime_error("The provided name of environment variable is empty.");
            }
        }

        class OptionalString {
           private:
            const char* str;

           public:
            OptionalString(const char* strInput) : str(strInput) {
            }

            bool isPresent() const {
                return str != nullptr;
            }
            const char* get() const {
                return str;
            }

            std::string getOrDefault(const std::string& defaultValue) const {
                return isPresent() ? get() : defaultValue;
            }
        };

        // Max value for an environment variable, according:
        // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getenvironmentvariable#parameters
        static constexpr std::size_t BUF_LEN = 32767;

        void setEnv(const std::string& name, const std::string& value) {
            assertNameIsNotEmpty(name);

            static constexpr int VALUE_FOR_REPLACE = 1;
            int result = setenv(name.c_str(), value.c_str(), VALUE_FOR_REPLACE);
            if (result != 0) {
                throw std::runtime_error("error - input contains '=' or not enough memory.");
            }
        }

        void unsetEnv(const std::string& name) {
            assertNameIsNotEmpty(name);

            int result = unsetenv(name.c_str());
            if (result != 0) {
                throw std::runtime_error("error - input contains '='.");
            }
        }

        std::string getEnv(const std::string& name) {
            assertNameIsNotEmpty(name);

            OptionalString result(getenv(name.c_str()));
            if (result.isPresent()) {
                return result.get();
            }

            throw std::runtime_error("error - no result found for " + name);
        }

        std::string getEnvOrDefault(const std::string& name, const std::string& defaultValue) {
            assertNameIsNotEmpty(name);

            OptionalString result(getenv(name.c_str()));
            return result.getOrDefault(defaultValue);
        }

        bool hasEnv(const std::string& name) {
            assertNameIsNotEmpty(name);

            OptionalString result(getenv(name.c_str()));
            return result.isPresent();
        }

        static void exploreEnviron(
            const std::function<void(char* thePair, char* positionOfEqualSign)>& function) {
            for (char** currentPair = environ; *currentPair != nullptr; currentPair++) {
                char* thePair = *currentPair;
                char* positionOfEqualSign = strchr(thePair, '=');
                function(thePair, positionOfEqualSign);
            }
        }

        std::vector<std::string> listNames() {
            std::vector<std::string> result;
            exploreEnviron([&result](char* thePair, char* positionOfEqualSign) {
                if (positionOfEqualSign == nullptr) {
                    // Abnormal!
                    return;
                }
                result.push_back(std::string(thePair, positionOfEqualSign - thePair));
            });

            return result;
        }

        std::vector<std::pair<std::string, std::string>> listAll() {
            std::vector<std::pair<std::string, std::string>> result;

            exploreEnviron([&result](char* thePair, char* positionOfEqualSign) {
                if (positionOfEqualSign == nullptr) {
                    // Abnormal!
                    return;
                }
                result.push_back(std::make_pair(
                    std::string(thePair, positionOfEqualSign - thePair),
                    std::string(positionOfEqualSign + 1)));
            });

            return result;
        }
    } // namespace Environment
} // namespace MF

#endif
