//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS

#    include <array>
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
            StreamItem fileStream = INVALID_HANDLE_VALUE;

            void beforeStart() override {
                fileStream = CreateFile(
                    KILL_FILENAME, // weirdly it does not work otherwise
                    FILE_GENERIC_WRITE, FILE_SHARE_READ, &getInheritableSecAttr(), OPEN_ALWAYS,
                    FILE_ATTRIBUTE_DEVICE, nullptr);
            }

            void afterStop() override {
                closeH(fileStream);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return fileStream;
            }

            ~ConsoleOutputChoice_Windows_Ignored() {
                closeH(fileStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored() {
            return std::make_shared<ConsoleOutputChoice_Windows_Ignored>();
        }

        struct ConsoleOutputChoice_Windows_File : ConsoleOutputChoice_Windows {
            StreamItem readFromFile = INVALID_HANDLE_VALUE;
            const Filename_t filename;

            ConsoleOutputChoice_Windows_File(const Filename_t &filename) : filename(filename) {
            }

            void beforeStart() override {
                readFromFile = CreateFile(
                    filename.c_str(), // weirdly it does not work otherwise
                    FILE_GENERIC_WRITE, FILE_SHARE_READ, &getInheritableSecAttr(), OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
            }

            void afterStop() override {
                closeH(readFromFile);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return readFromFile;
            }

            ~ConsoleOutputChoice_Windows_File() {
                closeH(readFromFile);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const Filename_t &filename) {
            return std::make_shared<ConsoleOutputChoice_Windows_File>(filename);
        }

        struct ConsoleOutputChoice_Windows_StringStream : ConsoleOutputChoice_Windows {
            std::stringstream &stringStream;

            StreamItem readStream = INVALID_HANDLE_VALUE;
            StreamItem writeToStream = INVALID_HANDLE_VALUE;

            ConsoleOutputChoice_Windows_StringStream(std::stringstream &stringStream1)
                : stringStream(stringStream1) {
            }

            void beforeStart() override {
                SECURITY_ATTRIBUTES securityAttributes{sizeof(SECURITY_ATTRIBUTES), nullptr, true};
                CreatePipe(&readStream, &writeToStream, &securityAttributes, 0);
                makeHandleInheritable(readStream, false);
                makeHandleInheritable(writeToStream, true);
            }

            void afterStart() override {
                closeH(writeToStream);
            }

            void beforeStop() override {
                static constexpr size_t BUFFER_SIZE = 4096;
                std::array<char, BUFFER_SIZE + 1> buffer{0};
                DWORD nbBytesRead = 0;
                do {
                    BOOL result =
                        ReadFile(readStream, buffer.data(), buffer.size(), &nbBytesRead, nullptr);
                    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
                    stringStream << buffer.data();
                } while (nbBytesRead == BUFFER_SIZE);
                closeH(readStream);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return writeToStream;
            }

            ~ConsoleOutputChoice_Windows_StringStream() {
                closeH(readStream);

                closeH(writeToStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToStringStream(std::stringstream &stream) {
            return std::make_shared<ConsoleOutputChoice_Windows_StringStream>(stream);
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
            const Filename_t inputString;
            StreamItem readStream = INVALID_HANDLE_VALUE;
            StreamItem writeToStream = INVALID_HANDLE_VALUE;

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

                closeH(writeToStream);
            }

            void afterStop() override {
                closeH(readStream);
            }

            StreamItem getStreamItem() const override {
                return readStream;
            }

            ~ConsoleInputChoice_Windows_String() {
                closeH(readStream);
                closeH(writeToStream);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::string &string) {
            return std::make_shared<ConsoleInputChoice_Windows_String>(string);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputEmpty() {
            return makeInputFromString("");
        }

        struct ConsoleInputChoice_Windows_File : ConsoleInputChoice_Windows {
            StreamItem readFromFile = INVALID_HANDLE_VALUE;
            const Filename_t filename;

            ConsoleInputChoice_Windows_File(const std::string &filename) : filename(filename) {
            }

            void beforeStart() override {
                readFromFile = CreateFile(
                    filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY,
                    NULL);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    readFromFile == INVALID_HANDLE_VALUE);
                makeHandleInheritable(readFromFile, true);
            }

            void afterStop() override {
                closeH(readFromFile);
            }

            StreamItem getStreamItem() const override {
                return readFromFile;
            }

            ~ConsoleInputChoice_Windows_File() {
                closeH(readFromFile);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const Filename_t &filename) {
            return std::make_shared<ConsoleInputChoice_Windows_File>(filename);
        }

        struct ConsoleInputChoice_Windows_StringStream : ConsoleInputChoice_Windows {
            std::stringstream &stringStream;

            StreamItem readStream = INVALID_HANDLE_VALUE;
            StreamItem writeToStream = INVALID_HANDLE_VALUE;

            ConsoleInputChoice_Windows_StringStream(std::stringstream &stringStream1)
                : stringStream(stringStream1) {
            }

            void beforeStart() override {
                SECURITY_ATTRIBUTES securityAttributes{sizeof(SECURITY_ATTRIBUTES), nullptr, true};
                CreatePipe(&readStream, &writeToStream, &securityAttributes, 0);
                makeHandleInheritable(readStream, true);
                makeHandleInheritable(writeToStream, false);
            }

            void afterStart() override {
                closeH(readStream);

                static constexpr size_t BUFFER_SIZE = 4096;
                std::array<char, BUFFER_SIZE + 1> buffer{0};
                DWORD nbBytesWritten = 0;
                do {
                    stringStream.get(buffer.data(), BUFFER_SIZE);
                    BOOL result = WriteFile(
                        writeToStream, buffer.data(), stringStream.gcount(), &nbBytesWritten,
                        nullptr);
                    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
                } while (nbBytesWritten == BUFFER_SIZE);

                closeH(writeToStream);
            }

            StreamItem getStreamItem() const override {
                return readStream;
            }

            ~ConsoleInputChoice_Windows_StringStream() {
                closeH(readStream);
                closeH(writeToStream);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromStringStream(std::stringstream &stream) {
            return std::make_shared<ConsoleInputChoice_Windows_StringStream>(stream);
        }
    } // namespace Command
} // namespace MF

#endif
