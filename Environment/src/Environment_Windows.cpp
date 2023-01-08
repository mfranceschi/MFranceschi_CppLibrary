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
        class EnvironmentBlockBase {
           protected:
            CharT* const block;
            CharT* firstEnvVar = block;

           protected:
            explicit EnvironmentBlockBase(CharT* block) : block(block) {
            }

           public:
            CharT* getFirstEnvVarEntry() const {
                return firstEnvVar;
            }

            virtual ~EnvironmentBlockBase() = default;
        };

        class EnvironmentBlockAnsi : public EnvironmentBlockBase<char> {
           public:
            EnvironmentBlockAnsi() : EnvironmentBlockBase<char>(GetEnvironmentStrings()) {
                while (*firstEnvVar == '=') {
                    firstEnvVar = nextEntry(firstEnvVar);
                }
            }

            static char* nextEntry(char* current) {
                return strchr(current, '\0') + 1;
            }

            static bool isOver(const char* current) {
                return *current == '\0';
            }

            ~EnvironmentBlockAnsi() {
                FreeEnvironmentStringsA(block);
            }
        };

        class EnvironmentBlockWide : public EnvironmentBlockBase<wchar_t> {
           public:
            EnvironmentBlockWide() : EnvironmentBlockBase<wchar_t>(GetEnvironmentStringsW()) {
                while (*firstEnvVar == L'=') {
                    firstEnvVar = nextEntry(firstEnvVar);
                }
            }

            static wchar_t* nextEntry(wchar_t* current) {
                return wcschr(current, L'\0') + 1;
            }

            static bool isOver(const wchar_t* current) {
                return *current == L'\0';
            }

            ~EnvironmentBlockWide() {
                FreeEnvironmentStringsW(block);
            }
        };

        template <typename CharT>
        using EnvironmentBlock_t = typename std::conditional<
            std::is_same<CharT, char>::value,
            EnvironmentBlockAnsi,
            EnvironmentBlockWide>::type;

        static char* nextOccurrence(char* str, int item) {
            return strchr(str, item);
        }

        static wchar_t* nextOccurrence(wchar_t* str, int item) {
            return wcschr(str, item);
        }

        template <typename CharT>
        static void exploreEnviron(
            const std::function<void(CharT* thePair, CharT* positionOfEqualSign)>& function) {
            EnvironmentBlock_t<CharT> environmentBlock;

            for (CharT* currentEntry = environmentBlock.getFirstEnvVarEntry();
                 !EnvironmentBlock_t<CharT>::isOver(currentEntry);
                 currentEntry = EnvironmentBlock_t<CharT>::nextEntry(currentEntry)) {
                CharT* name = currentEntry;
                CharT* positionOfEqualSign = nextOccurrence(name, '=');

                function(currentEntry, positionOfEqualSign);
            }
        }

        std::vector<std::string> listNames() {
            std::vector<std::string> result;
            exploreEnviron<char>([&result](char* thePair, char* positionOfEqualSign) {
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

            exploreEnviron<char>([&result](char* thePair, char* positionOfEqualSign) {
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
            std::vector<std::wstring> result;
            exploreEnviron<wchar_t>([&result](wchar_t* thePair, wchar_t* positionOfEqualSign) {
                if (positionOfEqualSign == nullptr) {
                    // Abnormal!
                    return;
                }
                result.push_back(std::wstring(thePair, positionOfEqualSign - thePair));
            });

            return result;
        }

        std::vector<std::pair<std::wstring, std::wstring>> listAllWide() {
            std::vector<std::pair<std::wstring, std::wstring>> result;

            exploreEnviron<wchar_t>([&result](wchar_t* thePair, wchar_t* positionOfEqualSign) {
                if (positionOfEqualSign == nullptr) {
                    // Abnormal!
                    return;
                }
                result.push_back(std::make_pair(
                    std::wstring(thePair, positionOfEqualSign - thePair),
                    std::wstring(positionOfEqualSign + 1)));
            });

            return result;
        }
    } // namespace Environment
} // namespace MF

#endif
