//
// Created by mfran on 09/04/2020.
//

#ifndef MYWORKS_TEST0_COMMAND_HPP
#define MYWORKS_TEST0_COMMAND_HPP

#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "MF/Filesystem.hpp"

namespace MF
{
    namespace Command
    {

        enum class OutputChoice {
            KEEP, // Let it on the console
            KILL, // Silent and ignore
            EXPORT, // Write outputs into the file "outputFile"
            EXPORT_APPEND, // Append outputs into the file "outputFile"
            RETRIEVE // Get outputs as string in the return structure
        };

        using ErrorChoice = OutputChoice;

        enum class InputChoice {
            NONE, // Nothing
            STRING, // Give the string "inputString"
            FROM_FILE // Gives the file named "inputString" as input
        };

        enum class ReturnChoice {
            WHEN_DONE, // Return the function right after it finishes (no new thread created)
            IMMEDIATELY, // The function returns now and the command runs independently
            FUNCTION // Same as IMMEDIATELY, but also calls the given function when done
        };

        enum class InterruptChoice {
            NEVER, // No restriction for the execution
            AFTER_TIME, // Interrupts the execution after the given time
            ON_DEMAND // Interrupts the execution when some function is called (returned in
                      // CommandReturn)
        };

        struct CommandReturn {
            int returnCode = 0; // Return value of the command
            std::string outputText; // [?] Complete string of the outputs
            std::string errorText; // [?] Complete string of the errors
            std::function<void()>
                callToTerminate; // [?] Call this to (try to) force the command to terminate
        };

        using Filesystem::SFilename_t;

        struct CommandCall {
            SFilename_t executable; // Name or path to the executable
            std::vector<SFilename_t> arguments; // List of arguments to the executable, they will be
                                                // concatenated with " ".
            SFilename_t outputFile; // [?] File in which to write outputs
            OutputChoice outputChoice = OutputChoice::KEEP; // Choice for outputs
            SFilename_t errorFile; // [?] File in which to write errors
            ErrorChoice errorChoice = ErrorChoice::KEEP; // Choice for errors
            SFilename_t inputString; // [?] String as input
            SFilename_t inputFile; // [?] File as input
            InputChoice inputChoice = InputChoice::NONE; // Choice for inputs
            std::function<void(CommandReturn &)> returnFunction;
            ReturnChoice returnChoice = ReturnChoice::WHEN_DONE;
            unsigned int executionDuration = -1; // In milliseconds
            InterruptChoice interruptChoice = InterruptChoice::NEVER;
        };

        void Command(const CommandCall &call, CommandReturn &);

        namespace Reboot
        {
            struct StdConsoleOutChoice {};

            struct OutputToFile : StdConsoleOutChoice {
               private:
                const Filename_t filename;
            };

            struct OutputToConsole : StdConsoleOutChoice {};

            struct OutputSilenced : StdConsoleOutChoice {};

            struct OutputToStringStream : StdConsoleOutChoice {
               private:
                std::istringstream iss;

               public:
                std::istringstream &getStringStream();
            };

            struct StdConsoleInChoice {};

            struct InputSomeFile : StdConsoleInChoice {
               private:
                const Filename_t filename;
            };

            struct InputSomeString : StdConsoleInChoice {
               private:
                const Filename_t text;
            };

            struct InputNothing : StdConsoleInChoice {};

            struct CommandCall2 {
                /**
                 * Name or path to the executable.
                 */
                Filename_t executable;

                /**
                 * List of arguments.
                 * The interpretation depends on the OS; we recommend using just 1 string.
                 */
                std::vector<Filename_t> arguments{};

                std::unique_ptr<StdConsoleOutChoice> stdOutChoice =
                    std::make_unique<OutputToConsole>();
                std::unique_ptr<StdConsoleOutChoice> stdErrChoice =
                    std::make_unique<OutputToConsole>();
                std::unique_ptr<StdConsoleInChoice> stdInChoice = std::make_unique<InputNothing>();
            };

            struct CommandOver2 {
                int exitCode = 0;
            };

            struct CommandAsyncReturn2 {
                std::future<CommandOver2> futureCommandOver;

                void tryToInterrupt();
                void wait();
                void waitFor(const std::chrono::duration &duration);
                void getReturnCode();
            };

            CommandAsyncReturn2 runCommandAsynchrone(const CommandCall2 &);
            CommandOver2 runCommandSynchrously(const CommandCall2 &);
        } // namespace Reboot

        // TODO implement
        // - Normal call
        // - CMD specific call
        // - PowerShell specific call
        // - Bash specific call
        // - Shell specific call
        // - ZSH specific call
        // - Script Bash
        // - Script PowerShell
        // - Script Shell
        // - Script Bash
        // - Get list of available stuff (bitfield: Bash, PowerShell, etc.)
        // - Common structure of parameters: quiet option, etc.

    } // namespace Command
} // namespace MF
#endif // MYWORKS_TEST0_COMMAND_HPP
