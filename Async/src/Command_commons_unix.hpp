//
// Created by MartinF on 08/05/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_UNIX_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_UNIX_HPP

#if MF_UNIX
#    include "Command_commons.hpp"

namespace MF
{
    namespace Command
    {
        void closeFd(int& fd);
    }
} // namespace MF
#endif

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_UNIX_HPP
