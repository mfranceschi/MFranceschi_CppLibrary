//
// Created by MartinF on 08/05/2023.
//

#if MF_UNIX

#    include <signal.h>
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <unistd.h>

#    include <cstring>

#    include "Command_commons_unix.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        using MF::SystemErrors::Errno;

        struct StatefulCommand_NotStartedYet : StatefulCommand_Base {
            StatefulCommand_NotStartedYet(const CommandCall &commandCall)
                : StatefulCommand_Base(
                      *(commandCall.stdInChoice),
                      *(commandCall.stdOutChoice),
                      *(commandCall.stdErrChoice)),
                  nbrArgs(commandCall.arguments.size()),
                  argv(new char *[nbrArgs + 2]),
                  file(commandCall.executable.c_str()) {
                // Arg 0 is the executable name, by convention.
                argv[0] = new char[commandCall.executable.size() + 1];
                std::strncpy(argv[0], file, commandCall.executable.size() + 1);

                // Last item of the 'argv' array is a NULL pointer to indicate the end.
                argv[nbrArgs + 1] = nullptr;

                for (std::size_t i = 0; i < nbrArgs; i++) {
                    const std::string &current = commandCall.arguments[i];
                    char *newArray = new char[current.size() + 1];
                    std::strncpy(newArray, current.c_str(), current.size());
                    newArray[current.size()] = '\0';

                    if (current[0] == '\"' && current[current.size() - 1] == '\"') {
                        // If the current arg string is surrounded by double quotes, remove
                        // those. TODO: does it make sense?
                        newArray[current.size() - 1] = '\0';
                        newArray++;
                    }
                    argv[i + 1] = newArray;
                }
            }

            ProcessItem start() override {
                beforeStart();

                const pid_t childProcessItem = fork();

                if (childProcessItem == 0) {
                    // Child process

                    dup2(stdInChoice.getStreamItemForStdIn(), STDIN_FILENO);
                    dup2(stdOutChoice.getStreamItemForStdOut(), STDOUT_FILENO);
                    dup2(stdErrChoice.getStreamItemForStdErr(), STDERR_FILENO);
                    /*processInputStream->closeOnFork();
                    processOutputStream->closeOnFork();
                    processErrorStream->closeOnFork();*/

                    execvp(file, argv);
                    throw Errno::getCurrentSystemError();
                } else {
                    // Parent process
                    ProcessItem processItem;
                    processItem.pid = childProcessItem;
                    return processItem;
                }
            }

            ~StatefulCommand_NotStartedYet() override {
                delete[] argv[0];
                for (size_t iArg = 0; iArg < nbrArgs; iArg++) {
                    delete[] argv[iArg + 1];
                }
                delete[] argv;
            }

           protected:
            void beforeStart() {
                stdInChoice.beforeStart();
                stdOutChoice.beforeStart();
                stdErrChoice.beforeStart();
            }

            void afterStart() {
                stdInChoice.afterStart();
                stdOutChoice.afterStart();
                stdErrChoice.afterStart();
            }

           private:
            const size_t nbrArgs;
            char **const argv;
            const char *file;
        };

        struct StatefulCommand_Running : StatefulCommand_Base {
            StatefulCommand_Running(
                ProcessItem &processItem,
                const std::shared_ptr<ConsoleInputChoice> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdErrChoice)
                : StatefulCommand_Base(*stdInChoice, *stdOutChoice, *stdErrChoice),
                  processItem(processItem) {
            }

            void kill() override {
                ::kill(processItem.pid, SIGKILL);
                ::waitpid(processItem.pid, &processItem.exitStatus, 0);
                processItem.hasWaited = true;
            }

            bool isRunning() override {
                // Calling 'kill' with a signal of 0 results in just checking that it still exists.
                // TODO: check that it's fine to use for a zombie process.
                return ::kill(processItem.pid, 0) == 0;
            }

            bool isDone() override {
                return !isRunning();
            }

            bool waitFor(
                std::chrono::milliseconds duration = std::chrono::milliseconds::zero()) override {
                if (duration == std::chrono::milliseconds::zero()) {
                    Errno::throwCurrentSystemErrorIf(
                        ::waitpid(processItem.pid, &processItem.exitStatus, 0) == -1);
                    processItem.hasWaited = true;
                    return true;
                }
                // TODO
                return false;
            }

            ~StatefulCommand_Running() override {
                afterStop();
            }

           private:
            void afterStop() {
                stdInChoice.afterStop();
                stdOutChoice.afterStop();
                stdErrChoice.afterStop();
            }

            // Data of the created process
            ProcessItem &processItem;
        };

        struct StatefulCommand_Over : StatefulCommand_Base {
            StatefulCommand_Over(
                const ProcessItem &processItem,
                const std::shared_ptr<ConsoleInputChoice> &stdInChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdOutChoice,
                const std::shared_ptr<ConsoleOutputChoice> &stdErrChoice)
                : StatefulCommand_Base(*stdInChoice, *stdOutChoice, *stdErrChoice) {
                commandOver.exitCode =
                    WIFEXITED(processItem.exitStatus)     ? WEXITSTATUS(processItem.exitStatus)
                    : WIFSIGNALED(processItem.exitStatus) ? WTERMSIG(processItem.exitStatus)
                                                          : throw std::runtime_error("WTF");
                // TODO differentiate paths between normal and abnormal terminations?
            }

            bool isDone() override {
                return true;
            }

            const CommandOver &getCommandOver() override {
                return commandOver;
            }

            ~StatefulCommand_Over() override = default;

           private:
            // Data of the terminated process
            CommandOver commandOver{-1};
        };

        CommandRunner_Proxy::CommandRunner_Proxy(const CommandCall &commandCall)
            : statefulCommandBase(std::make_unique<StatefulCommand_NotStartedYet>(commandCall)),
              stdInChoice(commandCall.stdInChoice),
              stdOutChoice(commandCall.stdOutChoice),
              stdErrChoice(commandCall.stdErrChoice) {
        }

        CommandRunner &CommandRunner_Proxy::start() {
            processItem = statefulCommandBase->start();
            statefulCommandBase = std::make_unique<StatefulCommand_Running>(
                processItem, stdInChoice, stdOutChoice, stdErrChoice);
            return *this;
        }

        CommandRunner &CommandRunner_Proxy::kill() {
            statefulCommandBase->kill();
            statefulCommandBase = std::make_unique<StatefulCommand_Over>(
                processItem, stdInChoice, stdOutChoice, stdErrChoice);
            return *this;
        }

        bool CommandRunner_Proxy::isRunning() {
            return statefulCommandBase->isRunning();
        }

        bool CommandRunner_Proxy::isDone() {
            return statefulCommandBase->isDone();
        }

        const CommandOver &CommandRunner_Proxy::getCommandOver() {
            return statefulCommandBase->getCommandOver();
        }

        bool CommandRunner_Proxy::waitFor(std::chrono::milliseconds duration) {
            const bool result = statefulCommandBase->waitFor(duration);
            if (result) {
                statefulCommandBase = std::make_unique<StatefulCommand_Over>(
                    processItem, stdInChoice, stdOutChoice, stdErrChoice);
            }
            return result;
        }

        void CommandRunner_Proxy::wait() {
            statefulCommandBase->waitFor(std::chrono::milliseconds::zero());
            statefulCommandBase = std::make_unique<StatefulCommand_Over>(
                processItem, stdInChoice, stdOutChoice, stdErrChoice);
        }

        std::uintmax_t CommandRunner_Proxy::getHandle() {
            return static_cast<uintmax_t>(processItem.pid);
        }

        CommandRunner_Proxy::~CommandRunner_Proxy() = default;
    } // namespace Command
} // namespace MF

#endif
