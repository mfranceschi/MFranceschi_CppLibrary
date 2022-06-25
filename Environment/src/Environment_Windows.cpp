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

        // Max value for an environment variable, according to:
        // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getenvironmentvariable#parameters
        static constexpr std::size_t BUF_LEN = 32767;

        class DwordAndBuffer {
           private:
            DWORD dword;
            std::shared_ptr<std::array<char, BUF_LEN>> buffer;

           public:
            DwordAndBuffer(DWORD dwordIn, decltype(buffer)& bufferIn)
                : dword(dwordIn), buffer(bufferIn) {
            }

            bool isError() const {
                return dword == 0;
            }

            std::string getString() const {
                return buffer->data();
            }
        };

        /**
         * See details here:
         * https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinea
         */
        class EnvironmentBlock {
           private:
            char* block;

           public:
            EnvironmentBlock() : block(GetEnvironmentStringsA()) {
            }

            ~EnvironmentBlock() {
                FreeEnvironmentStringsA(block);
            }
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
            if (dwordAndBuffer.isError()) {
                throw MF::Windows::GetCurrentSystemError();
            }

            return dwordAndBuffer.getString();
        }

        std::string getEnvOrDefault(const std::string& name, const std::string& defaultValue) {
            auto dwordAndBuffer = getEnvInternal(name);

            return (dwordAndBuffer.isError()) ? defaultValue : dwordAndBuffer.getString();
        }

        bool hasEnv(const std::string& name) {
            auto dwordAndBuffer = getEnvInternal(name);
            if (dwordAndBuffer.isError()) {
                auto systemError = MF::Windows::GetCurrentSystemError();
                if (systemError.code().value() == ERROR_ENVVAR_NOT_FOUND) {
                    return false;
                }
                throw systemError;
            }
            return true;
        }
    } // namespace Environment
} // namespace MF

#endif
