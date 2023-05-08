//
// Created by MartinF on 08/05/2023.
//

#include "Command_commons.hpp"

namespace MF
{
    namespace Command
    {
        StatefulCommand_Base::StatefulCommand_Base(
            ConsoleInputChoice &stdInChoice,
            ConsoleOutputChoice &stdOutChoice,
            ConsoleOutputChoice &stdErrChoice)
            : stdInChoice(stdInChoice), stdOutChoice(stdOutChoice), stdErrChoice(stdErrChoice) {
        }

        ProcessItem StatefulCommand_Base::start() {
            throw std::runtime_error("Unexpected call to " + std::string(__func__));
        }

        void StatefulCommand_Base::kill() {
            throw std::runtime_error("Unexpected call to " + std::string(__func__));
        }

        bool StatefulCommand_Base::isRunning() {
            return false;
        }

        bool StatefulCommand_Base::isDone() {
            return false;
        }

        const CommandOver &StatefulCommand_Base::getCommandOver() {
            throw std::runtime_error("Unexpected call to " + std::string(__func__));
        }

        bool StatefulCommand_Base::waitFor(std::chrono::milliseconds duration) {
            throw std::runtime_error("Unexpected call to " + std::string(__func__));
        }
    } // namespace Command
} // namespace MF
