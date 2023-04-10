//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS

#    include <cassert>

#    include "Command_Windows_commons.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        struct ConsoleOutputChoice_Windows_Console : ConsoleOutputChoice_Windows {
            StreamItem getStreamItem(OutputStream_e outputStream) const override {
                return GetStdHandle(
                    outputStream == OutputStream_e::StdOut ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToConsole() {
            return std::make_shared<ConsoleOutputChoice_Windows_Console>();
        }

        struct ConsoleOutputChoice_Windows_Ignored : ConsoleOutputChoice_Windows {
            static constexpr auto KILL_FILENAME = TEXT("NUL");
            StreamItem fileStream;

            void beforeStart() override {
                fileStream = CreateFile(
                    KILL_FILENAME, // weirdly it does not work otherwise
                    FILE_GENERIC_WRITE, FILE_SHARE_READ, &getInheritableSecAttr(), OPEN_ALWAYS,
                    FILE_ATTRIBUTE_DEVICE, nullptr);
            }

            void afterStop() override {
                CloseHandle(fileStream);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return fileStream;
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored() {
            return std::make_shared<ConsoleOutputChoice_Windows_Ignored>();
        }

        struct ConsoleInputChoice_Windows_Console : ConsoleInputChoice_Windows {
            StreamItem getStreamItem() const override {
                return GetStdHandle(STD_INPUT_HANDLE);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromConsole() {
            return std::make_shared<ConsoleInputChoice_Windows_Console>();
        }

        struct ConsoleInputChoice_Windows_String : ConsoleInputChoice_Windows {
            const std::string inputString; // TODO Unicode??
            StreamItem readStream = nullptr;
            StreamItem writeToStream = nullptr;

            ConsoleInputChoice_Windows_String(const std::string &string) : inputString(string) {
            }

            void beforeStart() override {
                SECURITY_ATTRIBUTES securityAttributes{sizeof(SECURITY_ATTRIBUTES), nullptr, true};
                CreatePipe(
                    &readStream, &writeToStream, &securityAttributes, inputString.length() + 1);
                makeHandleInheritable(readStream, true);
                makeHandleInheritable(writeToStream, false);
            }

            void afterStart() override {
                DWORD lpWritten;
                assert(inputString.length() < 1e3); // otherwise loop with a temp string buffer
                assert(WriteFile(
                    writeToStream, inputString.c_str(),
                    inputString.length() * sizeof(std::string::value_type), &lpWritten, nullptr));

                CloseHandle(writeToStream);
            }

            void afterStop() override {
                CloseHandle(readStream);
            }

            StreamItem getStreamItem() const override {
                return readStream;
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::string &string) {
            return std::make_shared<ConsoleInputChoice_Windows_String>(string);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputEmpty() {
            return makeInputFromString("");
        }
    } // namespace Command
} // namespace MF

#endif
