//
// Created by MartinF on 06/07/2023.
//

#if MF_UNIX

#    include <unistd.h>

#    include <array>
#    include <sstream>

#    include "Command_commons_unix.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        struct ConsoleOutputChoice_Unix_Console : ConsoleOutputChoice {
            StreamItem getStreamItemForStdOut() const override {
                return STDOUT_FILENO;
            }

            StreamItem getStreamItemForStdErr() const override {
                return STDERR_FILENO;
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToConsole() {
            return std::make_shared<ConsoleOutputChoice_Unix_Console>();
        }

        struct ConsoleOutputChoice_Unix_Ignored : ConsoleOutputChoice {
            StreamItem fileStream;

            ConsoleOutputChoice_Unix_Ignored() : fileStream(openNullFileToWrite()) {
            }

            void afterStop() override {
                closeFd(fileStream);
            }

            StreamItem getStreamItemForStdOut() const override {
                return fileStream;
            }

            ~ConsoleOutputChoice_Unix_Ignored() override {
                closeFd(fileStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored() {
            return std::make_shared<ConsoleOutputChoice_Unix_Ignored>();
        }

        struct ConsoleOutputChoice_Unix_File : ConsoleOutputChoice {
            StreamItem fileStream;

            ConsoleOutputChoice_Unix_File(const Filename_t &filename)
                : fileStream(openFileToWrite(filename)) {
            }

            void afterStop() override {
                closeFd(fileStream);
            }

            StreamItem getStreamItemForStdOut() const override {
                return fileStream;
            }

            ~ConsoleOutputChoice_Unix_File() override {
                closeFd(fileStream);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const Filename_t &filename) {
            return std::make_shared<ConsoleOutputChoice_Unix_File>(filename);
        }

        template <typename CharT>
        struct ConsoleOutputChoice_Unix_IOStream : ConsoleOutputChoice {
            std::basic_iostream<CharT> &stringStream;

            PipeStreams pipeStreams;

            ConsoleOutputChoice_Unix_IOStream(std::basic_iostream<CharT> &stringStream1)
                : stringStream(stringStream1), pipeStreams(makePipeThatChildWillWriteOn()) {
            }

            void afterStart() override {
                closeFd(pipeStreams.writeToPipe);
            }

            void afterStop() override {
                static constexpr size_t BUFFER_LENGTH = 4095;
                std::array<CharT, BUFFER_LENGTH + 1> buffer{0};

                ssize_t nbBytesRead = 0;
                do {
                    buffer.fill(0);
                    nbBytesRead = read(
                        pipeStreams.readFromPipe, buffer.data(), BUFFER_LENGTH * sizeof(CharT));
                    MF::SystemErrors::Errno::throwCurrentSystemErrorIf(nbBytesRead < 0);
                    stringStream << buffer.data();
                } while (nbBytesRead != 0);
                closeFd(pipeStreams.readFromPipe);
            }

            StreamItem getStreamItemForStdOut() const override {
                return pipeStreams.writeToPipe;
            }

            ~ConsoleOutputChoice_Unix_IOStream() override {
                closeFd(pipeStreams.readFromPipe);
                closeFd(pipeStreams.writeToPipe);
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToIOStream(
            std::basic_iostream<char> &stream) {
            return std::make_shared<ConsoleOutputChoice_Unix_IOStream<char>>(stream);
        }

        std::shared_ptr<ConsoleOutputChoice> makeOutputToIOStream(
            std::basic_iostream<wchar_t> &stream) {
            return std::make_shared<ConsoleOutputChoice_Unix_IOStream<wchar_t>>(stream);
        }

        struct ConsoleInputChoice_Unix_Console : ConsoleInputChoice {
            StreamItem getStreamItemForStdIn() const override {
                return STDIN_FILENO;
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromConsole() {
            return std::make_shared<ConsoleInputChoice_Unix_Console>();
        }

        std::shared_ptr<ConsoleInputChoice> makeInputEmpty() {
            return makeInputFromString("");
        }

        struct ConsoleInputChoice_Unix_File : ConsoleInputChoice {
            StreamItem readFromFile;

            ConsoleInputChoice_Unix_File(const Filename_t &filename)
                : readFromFile(openFileToRead(filename)) {
            }

            void afterStop() override {
                closeFd(readFromFile);
            }

            StreamItem getStreamItemForStdIn() const override {
                return readFromFile;
            }

            ~ConsoleInputChoice_Unix_File() override {
                closeFd(readFromFile);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const Filename_t &filename) {
            return std::make_shared<ConsoleInputChoice_Unix_File>(filename);
        }

        template <typename CharT>
        struct ConsoleInputChoice_Unix_IOStream : ConsoleInputChoice {
            std::basic_iostream<CharT> &stringStream;

            PipeStreams pipeStreams;

            ConsoleInputChoice_Unix_IOStream(std::basic_iostream<CharT> &stringStream1)
                : stringStream(stringStream1), pipeStreams(makePipeThatChildWillReadFrom()) {
            }

            void afterStart() override {
                closeFd(pipeStreams.readFromPipe);

                static constexpr size_t BUFFER_SIZE = 4096;
                std::array<CharT, BUFFER_SIZE + 1> buffer{0};
                ssize_t nbBytesWritten = 0;
                do {
                    stringStream.get(buffer.data(), BUFFER_SIZE);
                    nbBytesWritten = write(
                        pipeStreams.writeToPipe, buffer.data(),
                        stringStream.gcount() * sizeof(CharT));
                    MF::SystemErrors::Errno::throwCurrentSystemErrorIf(nbBytesWritten < 0);
                } while (nbBytesWritten != 0);

                closeFd(pipeStreams.writeToPipe);
            }

            StreamItem getStreamItemForStdIn() const override {
                return pipeStreams.readFromPipe;
            }

            ~ConsoleInputChoice_Unix_IOStream() override {
                closeFd(pipeStreams.readFromPipe);
                closeFd(pipeStreams.writeToPipe);
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromIOStream(
            std::basic_iostream<char> &stream) {
            return std::make_shared<ConsoleInputChoice_Unix_IOStream<char>>(stream);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputFromIOStream(
            std::basic_iostream<wchar_t> &stream) {
            return std::make_shared<ConsoleInputChoice_Unix_IOStream<wchar_t>>(stream);
        }

        template <typename CharT>
        struct ConsoleInputChoice_Unix_String : ConsoleInputChoice {
            std::basic_stringstream<CharT> stringStream;
            ConsoleInputChoice_Unix_IOStream<CharT> choiceWindowsStringStream;

            ConsoleInputChoice_Unix_String(const std::basic_string<CharT> &string)
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
            return std::make_shared<ConsoleInputChoice_Unix_String<char>>(string);
        }

        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::wstring &string) {
            return std::make_shared<ConsoleInputChoice_Unix_String<wchar_t>>(string);
        }
    } // namespace Command
} // namespace MF

#endif
