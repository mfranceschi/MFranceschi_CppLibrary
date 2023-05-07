//
// Created by MartinF on 08/05/2023.
//

#include "Command_commons.hpp"

namespace MF
{
    namespace Command
    {
        std::shared_ptr<CommandRunner> runCommandAsync(const CommandCall &commandCall) {
            return std::make_shared<CommandRunner_Proxy>(commandCall);
        }

        CommandOver runCommandAndWait(const CommandCall &commandCall) {
            auto cmd = runCommandAsync(commandCall);
            cmd->start().wait();
            return cmd->getCommandOver();
        }

#if MF_WINDOWS
        std::shared_ptr<CommandRunner> runCommandAsync(const WideCommandCall &commandCall) {
            return std::make_shared<CommandRunner_Proxy>(commandCall);
        }

        CommandOver runCommandAndWait(const WideCommandCall &commandCall) {
            auto cmd = runCommandAsync(commandCall);
            cmd->start().wait();
            return cmd->getCommandOver();
        }
#endif
    } // namespace Command
} // namespace MF
