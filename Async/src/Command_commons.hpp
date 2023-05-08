//
// Created by MartinF on 10/04/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_HPP

#include "MF/Command.hpp"
#include "MF/Commons.hpp"

#if MF_WINDOWS
#    include "MF/LightWindows.hpp"
#endif

namespace MF
{
    namespace Command
    {
#if MF_WINDOWS
        using StreamItem = HANDLE;
        using ProcessItem = HANDLE;
#else
        using StreamItem = int;
        using ProcessItem = pid_t;
#endif

        extern const StreamItem INVALID_STREAM_ITEM;

        struct ConsoleOutputChoice : Commons::NoCopy, Commons::NoMove {
            virtual void beforeStart() {
            }

            virtual void afterStart() {
            }

            virtual void afterStop() {
            }

            virtual StreamItem getStreamItemForStdOut() const = 0;
            virtual StreamItem getStreamItemForStdErr() const {
                return getStreamItemForStdOut();
            }
        };

        struct ConsoleInputChoice : Commons::NoCopy, Commons::NoMove {
            virtual void beforeStart() {
            }

            virtual void afterStart() {
            }

            virtual void afterStop() {
            }

            virtual StreamItem getStreamItemForStdIn() const = 0;
        };

        /**
         * Wrapper with named streams for input and output to the pipe.
         */
        struct PipeStreams {
            StreamItem writeToPipe = INVALID_STREAM_ITEM;
            StreamItem readFromPipe = INVALID_STREAM_ITEM;
        };

        PipeStreams makePipeThatChildWillReadFrom();
        PipeStreams makePipeThatChildWillWriteOn();

        StreamItem openFileToRead(const Filename_t &filename);
        StreamItem openFileToWrite(const Filename_t &filename);
        StreamItem openNullFileToWrite();

        struct StatefulCommand_Base;

        /**
         * Controller for any Command call.
         * Passes requests to the underlying StatefulCommand.
         */
        struct CommandRunner_Proxy : CommandRunner {
            CommandRunner_Proxy(const CommandCall &commandCall);
#if MF_WINDOWS
            CommandRunner_Proxy(const WideCommandCall &commandCall);
#endif
            CommandRunner &start() override;
            CommandRunner &kill(int exitCode) override;
            bool isRunning() override;
            bool isDone() override;
            const CommandOver &getCommandOver() override;
            bool waitFor(std::chrono::milliseconds duration) override;
            void wait() override;
            std::uintmax_t getHandle() override;
            ~CommandRunner_Proxy();

           private:
            std::unique_ptr<StatefulCommand_Base> statefulCommandBase;

            // Console choices
            std::shared_ptr<ConsoleInputChoice> stdInChoice;
            std::shared_ptr<ConsoleOutputChoice> stdOutChoice;
            std::shared_ptr<ConsoleOutputChoice> stdErrChoice;

            // Data of the created process
            ProcessItem processHandle = INVALID_STREAM_ITEM;
        };

        /**
         * The CommandRunner_Proxy uses an underlying state-controlled class.
         */
        struct StatefulCommand_Base {
            StatefulCommand_Base(
                ConsoleInputChoice &stdInChoice,
                ConsoleOutputChoice &stdOutChoice,
                ConsoleOutputChoice &stdErrChoice);

            virtual ProcessItem start();

            virtual void kill(int exitCode);

            virtual bool isRunning();

            virtual bool isDone();

            virtual const CommandOver &getCommandOver();

            virtual bool waitFor(std::chrono::milliseconds duration);

            virtual ~StatefulCommand_Base() = default;

            ConsoleInputChoice &stdInChoice;
            ConsoleOutputChoice &stdOutChoice;
            ConsoleOutputChoice &stdErrChoice;
        };
    } // namespace Command
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_HPP
