//
// Created by MartinF on 25/06/2022.
//

#if MF_WINDOWS

#    include <array>
#    include <functional>
#    include <memory>
#    include <stdexcept>

#    include "MF/Environment.hpp"
#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Environment
    {
        template <typename CharT>
        static void assertNameIsNotEmpty(const std::basic_string<CharT>& name) {
            if (name.empty()) {
                throw std::runtime_error("The provided name of environment variable is empty.");
            }
        }

        // Max value for an environment variable, according to:
        // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getenvironmentvariable#parameters
        static constexpr std::size_t BUF_LEN = 32767;

        template <typename CharT = char>
        class DwordAndBuffer {
           private:
            DWORD dword;
            std::shared_ptr<std::array<CharT, BUF_LEN>> buffer;

           public:
            DwordAndBuffer(DWORD dwordIn, decltype(buffer)& bufferIn)
                : dword(dwordIn), buffer(bufferIn) {
            }

            bool isError() const {
                return dword == 0;
            }

            std::basic_string<CharT> getString() const {
                return buffer->data();
            }
        };

        static DwordAndBuffer<> getEnvInternal(const std::string& name) {
            assertNameIsNotEmpty(name);

            auto chars = std::make_shared<std::array<char, BUF_LEN>>();
            std::memset(chars->data(), 0, BUF_LEN);

            DWORD result = GetEnvironmentVariableA(name.c_str(), chars->data(), BUF_LEN);
            return {result, chars};
        }

        static DwordAndBuffer<wchar_t> getEnvInternal(const std::wstring& name) {
            assertNameIsNotEmpty(name);

            auto chars = std::make_shared<std::array<wchar_t, BUF_LEN>>();
            std::memset(chars->data(), 0, BUF_LEN);

            DWORD result = GetEnvironmentVariableW(name.c_str(), chars->data(), BUF_LEN);
            return {result, chars};
        }

        void setEnv(const std::string& name, const std::string& value) {
            assertNameIsNotEmpty(name);

            BOOL result = SetEnvironmentVariableA(name.c_str(), value.c_str());
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(result == FALSE);
        }

        void unsetEnv(const std::string& name) {
            assertNameIsNotEmpty(name);

            BOOL result = SetEnvironmentVariableA(name.c_str(), nullptr);
            // It does not fail if it does not exist.
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(result == FALSE);
        }

        std::string getEnv(const std::string& name) {
            auto dwordAndBuffer = getEnvInternal(name);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(dwordAndBuffer.isError());

            return dwordAndBuffer.getString();
        }

        std::string getEnvOrDefault(const std::string& name, const std::string& defaultValue) {
            auto dwordAndBuffer = getEnvInternal(name);

            return (dwordAndBuffer.isError()) ? defaultValue : dwordAndBuffer.getString();
        }

        bool hasEnv(const std::string& name) {
            auto dwordAndBuffer = getEnvInternal(name);
            if (dwordAndBuffer.isError()) {
                auto systemError = MF::SystemErrors::Win32::getCurrentErrorCode();
                if (systemError == ERROR_ENVVAR_NOT_FOUND) {
                    return false;
                }
                throw MF::SystemErrors::Win32::getSystemErrorForErrorCode(systemError);
            }
            return true;
        }

#    ifdef GetEnvironmentStrings
        // We need this because the way it's done regarding macros.
        // We'll only use the ANSI version here.
#        undef GetEnvironmentStrings
#    endif

        /**
         * See details here:
         * https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinea
         */
        template <typename CharT>
        class EnvironmentBlock {
           private:
            CharT* const block;
            CharT* firstEnvVar = block;

           public:
            EnvironmentBlock() : block(GetEnvironmentStrings()) {
                while (*firstEnvVar == '=') {
                    firstEnvVar = nextEntry(firstEnvVar);
                }
            }

            CharT* getFirstEnvVarEntry() const {
                return firstEnvVar;
            }

            static CharT* nextEntry(CharT* current) {
                return strchr(current, '\0') + 1;
            }

            static bool isOver(const CharT* current) {
                return *current == '\0';
            }

            ~EnvironmentBlock() {
                FreeEnvironmentStringsA(block);
            }
        };

        static void exploreEnviron(
            const std::function<void(char* thePair, char* positionOfEqualSign)>& function) {
            EnvironmentBlock<char> environmentBlock;

            static const auto MyFreeEnvStringsA = [](char* ptr) {
                FreeEnvironmentStringsA(ptr);
            };

            auto ptr = std::unique_ptr<char, decltype(MyFreeEnvStringsA)>{
                GetEnvironmentStrings(), MyFreeEnvStringsA};

            for (char* currentEntry = environmentBlock.getFirstEnvVarEntry();
                 !EnvironmentBlock<char>::isOver(currentEntry);
                 currentEntry = EnvironmentBlock<char>::nextEntry(currentEntry)) {
                char* name = currentEntry;
                char* positionOfEqualSign = strchr(name, '=');

                function(currentEntry, positionOfEqualSign);
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

        void setEnv(const std::wstring& name, const std::wstring& value) {
            assertNameIsNotEmpty(name);

            BOOL result = SetEnvironmentVariableW(name.c_str(), value.c_str());
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(result == FALSE);
        }

        void unsetEnv(const std::wstring& name) {
            assertNameIsNotEmpty(name);

            BOOL result = SetEnvironmentVariableW(name.c_str(), nullptr);
            // It does not fail if it does not exist.
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(result == FALSE);
        }

        std::wstring getEnv(const std::wstring& name) {
            auto dwordAndBuffer = getEnvInternal(name);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(dwordAndBuffer.isError());

            return dwordAndBuffer.getString();
        }

        std::wstring getEnvOrDefault(const std::wstring& name, const std::wstring& defaultValue) {
            auto dwordAndBuffer = getEnvInternal(name);

            return (dwordAndBuffer.isError()) ? defaultValue : dwordAndBuffer.getString();
        }

        bool hasEnv(const std::wstring& name) {
            auto dwordAndBuffer = getEnvInternal(name);
            if (dwordAndBuffer.isError()) {
                auto systemError = MF::SystemErrors::Win32::getCurrentErrorCode();
                if (systemError == ERROR_ENVVAR_NOT_FOUND) {
                    return false;
                }
                throw MF::SystemErrors::Win32::getSystemErrorForErrorCode(systemError);
            }
            return true;
        }

        std::vector<std::wstring> listNamesWide() {
            // TODO
            return {};
        }

        std::vector<std::pair<std::wstring, std::wstring>> listAllWide() {
            // TODO
            return {};
        }
    } // namespace Environment
} // namespace MF

#endif
