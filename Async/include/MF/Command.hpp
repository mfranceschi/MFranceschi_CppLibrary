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
            int exitCode;

            bool hasSucceeded() const {
                return exitCode == EXIT_SUCCESS;
            }
        };

        struct CommandRunner {
            /**
             * Starts the process.
             * @return this
             */
            virtual CommandRunner &start() = 0;

            /**
             * Kills the process, and only returns once the process has been successfully killed.
             * Windows: uses "TerminateProcess" and "exitCode" is the exit code of the process.
             * Unix: uses "kill" to send a SIGKILL to the process; "exitCode" is ignored.
             */
            virtual CommandRunner &kill(int exitCode = -1) = 0;

            /**
             * Returns true if the process has been started and is not finished yet.
             */
            virtual bool isRunning() = 0;

            /**
             * Returns true if the process has been started and is finished.
             */
            virtual bool isDone() = 0;

            /**
             * Waits for the specified duration.
             * Returns true as soon as the process is finished.
             * Returns false if the process is still running after that duration.
             */
            virtual bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) = 0;

            /**
             * If the process is finished, returns the corresponding CommandOver.
             * Otherwise throws a runtime error.
             */
            virtual const CommandOver &getCommandOverOrThrow() = 0;

            /**
             * Returns the process handle.
             * Windows: HANDLE
             * Unix: pid_t
             */
            virtual std::uintmax_t getHandle() = 0;

            virtual ~CommandRunner() = default;
        };

        std::shared_ptr<CommandRunner> runCommandAsync(const CommandCall &commandCall);

        CommandOver runCommandAndWait(
            const CommandCall &commandCall,
            std::chrono::milliseconds waitFor = std::chrono::milliseconds::zero());
    } // namespace Command
} // namespace MF
#endif // MYWORKS_TEST0_COMMAND_HPP
