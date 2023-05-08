//
// Created by MartinF on 08/05/2023.
//

#if MF_UNIX

#    include <signal.h>
#    include <sys/types.h>
#    include <sys/wait.h>

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
                      *(commandCall.stdErrChoice)) {
            }

            ProcessItem start() override {
                beforeStart();

                // TODO

                throw std::runtime_error("Not yet implemented!");
            }

            ~StatefulCommand_NotStartedYet() override = default;

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
