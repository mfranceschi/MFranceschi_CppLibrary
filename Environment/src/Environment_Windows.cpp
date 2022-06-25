//
// Created by MartinF on 25/06/2022.
//

#if MF_WINDOWS

#    include <array>
#    include <memory>
#    include <stdexcept>

#    include "MF/Environment.hpp"
#    include "MF/LightWindows.hpp"
#    include "MF/Windows.hpp"

namespace MF
{
    namespace Environment
    {
        static void assertNameIsNotEmpty(const std::string& name) {
            if (name.empty()) {
                throw std::runtime_error("The provided name of environment variable is empty.");
            }
        }

        // Max value for an environment variable, according:
        // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getenvironmentvariable#parameters
        static constexpr std::size_t BUF_LEN = 32767;

        struct DwordAndBuffer {
            DWORD dword;
            std::shared_ptr<std::array<char, BUF_LEN>> buffer;
        };

        static DwordAndBuffer getEnvInternal(const std::string& name) {
            assertNameIsNotEmpty(name);

            std::shared_ptr<std::array<char, BUF_LEN>> chars =
                std::make_shared<std::array<char, BUF_LEN>>();
            std::memset(chars->data(), 0, BUF_LEN);

            DWORD result = GetEnvironmentVariableA(name.c_str(), chars->data(), BUF_LEN);
            return {result, chars};
        }

        void setEnv(const std::string& name, const std::string& value) {
            assertNameIsNotEmpty(name);

            BOOL result = SetEnvironmentVariable(name.c_str(), value.c_str());
            if (result == FALSE) {
                throw MF::Windows::GetCurrentSystemError();
            }
        }

        void unsetEnv(const std::string& name) {
            assertNameIsNotEmpty(name);

            BOOL result = SetEnvironmentVariable(name.c_str(), nullptr);
            // It does not fail if it does not exist.
            if (result == FALSE) {
                throw MF::Windows::GetCurrentSystemError();
            }
        }

        std::string getEnv(const std::string& name) {
            auto dwordAndBuffer = getEnvInternal(name);
            if (dwordAndBuffer.dword == 0) {
                throw MF::Windows::GetCurrentSystemError();
            }

            return dwordAndBuffer.buffer->data();
        }

        std::string getEnvOrDefault(const std::string& name, const std::string& defaultValue) {
            auto dwordAndBuffer = getEnvInternal(name);

            return (dwordAndBuffer.dword == 0) ? defaultValue : dwordAndBuffer.buffer->data();
        }
    } // namespace Environment
} // namespace MF

#endif
