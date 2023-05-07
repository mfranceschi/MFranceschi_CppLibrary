//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS

#    include <array>
#    include <sstream>

#    include "Command_commons_windows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Windows.hpp"

namespace MF
{
    namespace Command
    {
        struct ConsoleOutputChoice_Windows_Console : ConsoleOutputChoice {
            StreamItem getStreamItemForStdOut() const override {
                return GetStdHandle(STD_OUTPUT_HANDLE);
            }

            StreamItem getStreamItemForStdErr() const override {
                return GetStdHandle(STD_ERROR_HANDLE);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToConsole() {
            return std::make_shared<ConsoleOutputChoice_Windows_Console>();
        }

        struct ConsoleOutputChoice_Windows_Ignored : ConsoleOutputChoice {
            StreamItem fileStream;

            ConsoleOutputChoice_Windows_Ignored() : fileStream(openNullFileToWrite()) {
            }

            void afterStop() override {
                closeH(fileStream);
            }

            StreamItem getStreamItemForStdOut() const override {
                return fileStream;
            }

            ~ConsoleOutputChoice_Windows_Ignored() override {
                closeH(fileStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored() {
            return std::make_shared<ConsoleOutputChoice_Windows_Ignored>();
        }

        struct ConsoleOutputChoice_Windows_File : ConsoleOutputChoice {
            StreamItem fileStream;

            ConsoleOutputChoice_Windows_File(const Filename_t &filename)
                : fileStream(openFileToWrite(filename)) {
            }

            ConsoleOutputChoice_Windows_File(const WideFilename_t &filename)
                : fileStream(openFileToWrite(filename)) {
            }

            void afterStop() override {
                closeH(fileStream);
            }

            StreamItem getStreamItemForStdOut() const override {
                return fileStream;
            }

            ~ConsoleOutputChoice_Windows_File() override {
                closeH(fileStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const Filename_t &filename) {
            return std::make_shared<ConsoleOutputChoice_Windows_File>(filename);
        }

        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const WideFilename_t &filename) {
            return std::make_shared<ConsoleOutputChoice_Windows_File>(filename);
        }

        template <typename CharT>
        struct ConsoleOutputChoice_Windows_IOStream : ConsoleOutputChoice {
            std::basic_iostream<CharT> &stringStream;

            PipeStreams pipeStreams;

            ConsoleOutputChoice_Windows_IOStream(std::basic_iostream<CharT> &stringStream1)
                : stringStream(stringStream1), pipeStreams(makePipeThatChildWillWriteOn()) {
            }

            void afterStart() override {
                closeH(pipeStreams.writeToPipe);
            }

            void afterStop() override {
                static constexpr size_t BUFFER_LENGTH = 4095;
                std::array<CharT, BUFFER_LENGTH + 1> buffer{0};
                DWORD nbBytesRead = 0;
                do {
                    BOOL result = ReadFile(
                        pipeStreams.readFromPipe, buffer.data(), BUFFER_LENGTH * sizeof(CharT),
                        &nbBytesRead, nullptr);
                    if (!result) {
                        auto lastError = MF::SystemErrors::Win32::getCurrentErrorCode();
                        if (lastError == ERROR_BROKEN_PIPE) {
                            // It means that the pipe has already been closed on its WRITE end,
                            // aka we have nothing to read here.
                            // https://devblogs.microsoft.com/oldnewthing/20190405-00/?p=102389
                            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile#pipes
                            break;
                        } else {
                            throw MF::SystemErrors::Win32::getSystemErrorForErrorCode(lastError);
                        }
                    }
                    stringStream << buffer.data();
                } while (nbBytesRead == BUFFER_LENGTH * sizeof(CharT));
                closeH(pipeStreams.readFromPipe);
            }

            StreamItem getStreamItemForStdOut() const override {
                return pipeStreams.writeToPipe;
            }

            ~ConsoleOutputChoice_Windows_IOStream() override {
                closeH(pipeStreams.readFromPipe);
                closeH(pipeStreams.writeToPipe);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToIOStream(
            std::basic_iostream<char> &stream) {
            return std::make_shared<ConsoleOutputChoice_Windows_IOStream<char>>(stream);
        }

        std::shared_ptr<ConsoleOutputChoice> makeOutputToIOStream(
            std::basic_iostream<wchar_t> &stream) {
            return std::make_shared<ConsoleOutputChoice_Windows_IOStream<wchar_t>>(stream);
        }

        struct ConsoleInputChoice_Windows_Console : ConsoleInputChoice {
            StreamItem getStreamItemForStdIn() const override {
                return GetStdHandle(STD_INPUT_HANDLE);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromConsole() {
            return std::make_shared<ConsoleInputChoice_Windows_Console>();
        }

        std::shared_ptr<ConsoleInputChoice> makeInputEmpty() {
            return makeInputFromString("");
        }

        struct ConsoleInputChoice_Windows_File : ConsoleInputChoice {
            StreamItem readFromFile;

            ConsoleInputChoice_Windows_File(const Filename_t &filename)
                : readFromFile(openFileToRead(filename)) {
                Windows::Handles::makeHandleInheritable(readFromFile, true);
            }

            ConsoleInputChoice_Windows_File(const WideFilename_t &filename)
                : readFromFile(openFileToRead(filename)) {
                Windows::Handles::makeHandleInheritable(readFromFile, true);
            }

            void afterStop() override {
                closeH(readFromFile);
            }

            StreamItem getStreamItemForStdIn() const override {
                return readFromFile;
            }

            ~ConsoleInputChoice_Windows_File() override {
                closeH(readFromFile);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const Filename_t &filename) {
            return std::make_shared<ConsoleInputChoice_Windows_File>(filename);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const WideFilename_t &filename) {
            return std::make_shared<ConsoleInputChoice_Windows_File>(filename);
        }

        template <typename CharT>
        struct ConsoleInputChoice_Windows_IOStream : ConsoleInputChoice {
            std::basic_iostream<CharT> &stringStream;

            PipeStreams pipeStreams;

            ConsoleInputChoice_Windows_IOStream(std::basic_iostream<CharT> &stringStream1)
                : stringStream(stringStream1), pipeStreams(makePipeThatChildWillReadFrom()) {
            }

            void afterStart() override {
                closeH(pipeStreams.readFromPipe);

                static constexpr size_t BUFFER_SIZE = 4096;
                std::array<CharT, BUFFER_SIZE + 1> buffer{0};
                DWORD nbBytesWritten = 0;
                do {
                    stringStream.get(buffer.data(), BUFFER_SIZE);
                    BOOL result = WriteFile(
                        pipeStreams.writeToPipe, buffer.data(),
                        stringStream.gcount() * sizeof(CharT), &nbBytesWritten, nullptr);
                    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
                } while (nbBytesWritten == BUFFER_SIZE);

                closeH(pipeStreams.writeToPipe);
            }

            StreamItem getStreamItemForStdIn() const override {
                return pipeStreams.readFromPipe;
            }

            ~ConsoleInputChoice_Windows_IOStream() override {
                closeH(pipeStreams.readFromPipe);
                closeH(pipeStreams.writeToPipe);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromIOStream(
            std::basic_iostream<char> &stream) {
            return std::make_shared<ConsoleInputChoice_Windows_IOStream<char>>(stream);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputFromIOStream(
            std::basic_iostream<wchar_t> &stream) {
            return std::make_shared<ConsoleInputChoice_Windows_IOStream<wchar_t>>(stream);
        }

        template <typename CharT>
        struct ConsoleInputChoice_Windows_String : ConsoleInputChoice {
            std::basic_stringstream<CharT> stringStream;
            ConsoleInputChoice_Windows_IOStream<CharT> choiceWindowsStringStream;

            ConsoleInputChoice_Windows_String(const std::basic_string<CharT> &string)
                : stringStream(string), choiceWindowsStringStream(stringStream) {
            }

            void beforeStart() override {
                choiceWindowsStringStream.beforeStart();
            }

            void afterStart() override {
                choiceWindowsStringStream.afterStart();
            }

            void afterStop() override {
                choiceWindowsStringStream.afterStop();
            }

            StreamItem getStreamItemForStdIn() const override {
                return choiceWindowsStringStream.getStreamItemForStdIn();
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::string &string) {
            return std::make_shared<ConsoleInputChoice_Windows_String<char>>(string);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::wstring &string) {
            return std::make_shared<ConsoleInputChoice_Windows_String<wchar_t>>(string);
        }
    } // namespace Command
} // namespace MF

#endif
