//
// Created by mfran on 09/04/2020.
//

#ifndef MYWORKS_TEST0_COMMAND_HPP
#define MYWORKS_TEST0_COMMAND_HPP

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
        std::shared_ptr<ConsoleOutputChoice> makeOutputToIStringStream(
            const std::istringstream &stream);
        std::shared_ptr<ConsoleOutputChoice> makeOutputToConsole();
        std::shared_ptr<ConsoleOutputChoice> makeOutputIgnored();

        struct ConsoleInputChoice;
        std::shared_ptr<ConsoleInputChoice> makeInputFromFile(const Filename_t &filename);
        std::shared_ptr<ConsoleInputChoice> makeInputFromString(const std::string &string);
        std::shared_ptr<ConsoleInputChoice> makeInputFromOStringStream(
            const std::ostringstream &stream);
        std::shared_ptr<ConsoleInputChoice> makeInputFromConsole();
        std::shared_ptr<ConsoleInputChoice> makeInputEmpty();

        struct CommandCall {
            /**
             * Name or path to the executable.
             */
            Filename_t executable;

            /**
             * List of arguments.
             * The interpretation depends on the OS; we recommend using just 1 string.
             */
            std::vector<Filename_t> arguments{};

            std::shared_ptr<ConsoleOutputChoice> stdOutChoice = makeOutputToConsole();
            std::shared_ptr<ConsoleOutputChoice> stdErrChoice = makeOutputToConsole();
            std::shared_ptr<ConsoleInputChoice> stdInChoice = makeInputFromConsole();
        };

        struct CommandOver {
            const int exitCode;
        };

        struct CommandAsyncReturn {
            std::future<CommandOver> futureCommandOver;

            void tryToInterrupt();
            void wait();
            void waitFor(const std::chrono::nanoseconds &duration);
            void getReturnCode();
        };

        CommandAsyncReturn runCommandAsync(const CommandCall &commandCall);
        CommandOver runCommandAndWait(const CommandCall &commandCall);
    } // namespace Command
} // namespace MF
#endif // MYWORKS_TEST0_COMMAND_HPP
