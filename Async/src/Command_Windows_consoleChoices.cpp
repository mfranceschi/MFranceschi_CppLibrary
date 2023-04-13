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
            StreamItem fileStream = INVALID_HANDLE_VALUE;

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

            ~ConsoleOutputChoice_Windows_Ignored() {
                if (fileStream != INVALID_HANDLE_VALUE) {
                    CloseHandle(fileStream);
                }
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored() {
            return std::make_shared<ConsoleOutputChoice_Windows_Ignored>();
        }

        struct ConsoleOutputChoice_Windows_File : ConsoleOutputChoice_Windows {
            StreamItem readFromFile = INVALID_HANDLE_VALUE;
            const Filename_t filename; // TODO Unicode??

            ConsoleOutputChoice_Windows_File(const Filename_t &filename) : filename(filename) {
            }

            void beforeStart() override {
                readFromFile = CreateFile(
                    filename.c_str(), // weirdly it does not work otherwise
                    FILE_GENERIC_WRITE, FILE_SHARE_READ, &getInheritableSecAttr(), OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
            }

            void afterStop() override {
                CloseHandle(readFromFile);
            }

            StreamItem getStreamItem(OutputStream_e) const override {
                return readFromFile;
            }

            ~ConsoleOutputChoice_Windows_File() {
                if (readFromFile != INVALID_HANDLE_VALUE) {
                    CloseHandle(readFromFile);
                }
            }
        };

        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const Filename_t &filename) {
            return std::make_shared<ConsoleOutputChoice_Windows_File>(filename);
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

                CloseHandle(writeToStream);
            }

            void afterStop() override {
                CloseHandle(readStream);
            }

            StreamItem getStreamItem() const override {
                return readStream;
            }

            ~ConsoleInputChoice_Windows_String() {
                if (readStream != INVALID_HANDLE_VALUE) {
                    CloseHandle(readStream);
                }
                if (writeToStream != INVALID_HANDLE_VALUE) {
                    CloseHandle(writeToStream);
                }
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
            StreamItem writeToPipe = INVALID_HANDLE_VALUE;
            StreamItem readFromPipe = INVALID_HANDLE_VALUE;

            const std::string filename; // TODO Unicode??
            static constexpr size_t BUFFER_SIZE = 4096;

            ConsoleInputChoice_Windows_File(const std::string &filename) : filename(filename) {
            }

            void beforeStart() override {
                SECURITY_ATTRIBUTES securityAttributes{sizeof(SECURITY_ATTRIBUTES), nullptr, true};
                assert(CreatePipe(&readFromPipe, &writeToPipe, &securityAttributes, 0));
                makeHandleInheritable(readFromPipe, true);
                makeHandleInheritable(writeToPipe, false);

                readFromFile = CreateFile(
                    filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY,
                    NULL);
                MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                    readFromFile == INVALID_HANDLE_VALUE);
                makeHandleInheritable(readFromFile, false);
            }

            void afterStart() override {
                // Copy-pasted from
                // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output
                DWORD nbrBytesRead, nbrBytesWritten;
                CHAR chBuf[BUFFER_SIZE];
                BOOL bSuccess = FALSE;

                for (;;) {
                    bSuccess = ReadFile(readFromFile, chBuf, BUFFER_SIZE, &nbrBytesRead, NULL);
                    if (!bSuccess || nbrBytesRead == 0) break;

                    bSuccess = WriteFile(writeToPipe, chBuf, nbrBytesRead, &nbrBytesWritten, NULL);
                    if (!bSuccess) break;
                }

                CloseHandle(writeToPipe);
                CloseHandle(readFromFile);
            }

            void afterStop() override {
                CloseHandle(readFromPipe);
            }

            StreamItem getStreamItem() const override {
                return readFromPipe;
            }

            ~ConsoleInputChoice_Windows_File() {
                if (readFromFile != INVALID_HANDLE_VALUE) {
                    CloseHandle(readFromFile);
                }
                if (writeToPipe != INVALID_HANDLE_VALUE) {
                    CloseHandle(writeToPipe);
                }
                if (readFromPipe != INVALID_HANDLE_VALUE) {
                    CloseHandle(readFromPipe);
                }
            }
        };

        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const Filename_t &filename) {
            return std::make_shared<ConsoleInputChoice_Windows_File>(filename);
        }
    } // namespace Command
} // namespace MF

#endif
