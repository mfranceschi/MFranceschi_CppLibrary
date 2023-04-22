//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS

#    include <array>

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
            StreamItem fileStream;

            ConsoleOutputChoice_Windows_Ignored() : fileStream(openNullFileToWrite()) {
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
            StreamItem fileStream;

            ConsoleOutputChoice_Windows_File(const Filename_t &filename)
                : fileStream(openFileToWrite(filename)) {
            }

            void afterStop() override {
                closeH(fileStream);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return fileStream;
            }

            ~ConsoleOutputChoice_Windows_File() {
                closeH(fileStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const Filename_t &filename) {
            return std::make_shared<ConsoleOutputChoice_Windows_File>(filename);
        }

        struct ConsoleOutputChoice_Windows_StringStream : ConsoleOutputChoice_Windows {
            std::stringstream &stringStream;

            PipeStreams pipeStreams;

            ConsoleOutputChoice_Windows_StringStream(std::stringstream &stringStream1)
                : stringStream(stringStream1), pipeStreams(makePipeThatChildWillWriteOn()) {
            }

            void afterStart() override {
                closeH(pipeStreams.writeToPipe);
            }

            void afterStop() override {
                static constexpr size_t BUFFER_SIZE = 4096;
                std::array<char, BUFFER_SIZE + 1> buffer{0};
                DWORD nbBytesRead = 0;
                do {
                    BOOL result = ReadFile(
                        pipeStreams.readFromPipe, buffer.data(), buffer.size(), &nbBytesRead,
                        nullptr);
                    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
                    stringStream << buffer.data();
                } while (nbBytesRead == BUFFER_SIZE);
                closeH(pipeStreams.readFromPipe);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return pipeStreams.writeToPipe;
            }

            ~ConsoleOutputChoice_Windows_StringStream() {
                closeH(pipeStreams.readFromPipe);
                closeH(pipeStreams.writeToPipe);
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

        std::shared_ptr<ConsoleInputChoice> makeInputEmpty() {
            return makeInputFromString("");
        }

        struct ConsoleInputChoice_Windows_File : ConsoleInputChoice_Windows {
            StreamItem readFromFile;

            ConsoleInputChoice_Windows_File(const std::string &filename)
                : readFromFile(openFileToRead(filename)) {
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

            PipeStreams pipeStreams;

            ConsoleInputChoice_Windows_StringStream(std::stringstream &stringStream1)
                : stringStream(stringStream1), pipeStreams(makePipeThatChildWillRead()) {
            }

            void afterStart() override {
                closeH(pipeStreams.readFromPipe);

                static constexpr size_t BUFFER_SIZE = 4096;
                std::array<char, BUFFER_SIZE + 1> buffer{0};
                DWORD nbBytesWritten = 0;
                do {
                    stringStream.get(buffer.data(), BUFFER_SIZE);
                    BOOL result = WriteFile(
                        pipeStreams.writeToPipe, buffer.data(), stringStream.gcount(),
                        &nbBytesWritten, nullptr);
                    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
                } while (nbBytesWritten == BUFFER_SIZE);

                closeH(pipeStreams.writeToPipe);
            }

            StreamItem getStreamItem() const override {
                return pipeStreams.readFromPipe;
            }

            ~ConsoleInputChoice_Windows_StringStream() {
                closeH(pipeStreams.readFromPipe);
                closeH(pipeStreams.writeToPipe);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromStringStream(std::stringstream &stream) {
            return std::make_shared<ConsoleInputChoice_Windows_StringStream>(stream);
        }

        struct ConsoleInputChoice_Windows_String : ConsoleInputChoice_Windows {
            std::stringstream stringStream;
            ConsoleInputChoice_Windows_StringStream choiceWindowsStringStream;

            ConsoleInputChoice_Windows_String(const std::string &string)
                : stringStream(string), choiceWindowsStringStream(stringStream) {
            }

            void beforeStart() override {
                choiceWindowsStringStream.beforeStart();
            }

            void afterStart() override {
                choiceWindowsStringStream.afterStart();
            }

            void beforeStop() override {
                choiceWindowsStringStream.beforeStop();
            }

            void afterStop() override {
                choiceWindowsStringStream.afterStop();
            }

            StreamItem getStreamItem() const override {
                return choiceWindowsStringStream.getStreamItem();
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::string &string) {
            return std::make_shared<ConsoleInputChoice_Windows_String>(string);
        }
    } // namespace Command
} // namespace MF

#endif
