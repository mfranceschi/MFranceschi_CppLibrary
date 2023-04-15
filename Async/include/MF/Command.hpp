//
// Created by mfran on 09/04/2020.
//

#ifndef MYWORKS_TEST0_COMMAND_HPP
#define MYWORKS_TEST0_COMMAND_HPP

#include <chrono>
#include <functional>
#include <future>
#include <sstream>
#include <string>
#include <vector>

#include "MF/Filesystem.hpp"

namespace MF
{
    namespace Command
    {
        using Filesystem::Filename_t;

        struct ConsoleOutputChoice;
        std::shared_ptr<ConsoleOutputChoice> makeOutputToFile(const Filename_t &filename);
        std::shared_ptr<ConsoleOutputChoice> makeOutputToStringStream(std::stringstream &stream);
        std::shared_ptr<ConsoleOutputChoice> makeOutputToConsole();
        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored();

        struct ConsoleInputChoice;
        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const Filename_t &filename);
        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::string &string);
        std::shared_ptr<ConsoleInputChoice> makeInputFromStringStream(std::stringstream &stream);
        std::shared_ptr<ConsoleInputChoice> makeInputFromConsole();
        std::shared_ptr<ConsoleInputChoice> makeInputEmpty();

        struct CommandCall {
            /**
             * Name or path to the executable.
             */
            Filename_t executable;

            /**
             * List of arguments.
             * The interpretation may depend on the OS.
             */
            std::vector<std::string> arguments{};

            /**
             * Expected 'current working directory' of the child process.
             * Empty = same as parent.
             */
            Filename_t workingDirectory;

            std::shared_ptr<ConsoleOutputChoice> stdOutChoice = makeOutputToConsole();
            std::shared_ptr<ConsoleOutputChoice> stdErrChoice = makeOutputToConsole();
            std::shared_ptr<ConsoleInputChoice> stdInChoice = makeInputFromConsole();
        };

        struct CommandOver {
            const int exitCode;

            bool hasSucceeded() const {
                return exitCode == EXIT_SUCCESS;
            }
        };

        struct CommandRunner {
            virtual CommandRunner &start() = 0;
            virtual CommandRunner &sendStop() = 0;
            virtual bool isRunning() = 0;
            virtual bool isStopped() = 0;
            virtual CommandOver &getCommandOverOrThrow() = 0;
            virtual bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) = 0;
        };

        struct CommandAsyncReturn { // TODO NOT THIS
            virtual void tryToStop() = 0;
            virtual void wait() = 0;
            virtual void waitFor(const std::chrono::nanoseconds &duration) = 0;
            virtual std::future<CommandOver> getFutureCommandOver() = 0;
        };

        std::shared_ptr<CommandAsyncReturn> runCommandAsync(const CommandCall &commandCall);

        CommandOver runCommandAndWait(
            const CommandCall &commandCall,
            std::chrono::milliseconds waitFor = std::chrono::milliseconds::zero());
    } // namespace Command
} // namespace MF
#endif // MYWORKS_TEST0_COMMAND_HPP
