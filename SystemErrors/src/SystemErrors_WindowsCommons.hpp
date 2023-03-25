//
// Created by MartinF on 31/07/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_WINDOWSCOMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_WINDOWSCOMMONS_HPP

#if MF_WINDOWS

#    include <string>
#    include <system_error>

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace SystemErrors
    {
        namespace WindowsCommons
        {
            std::string getErrorMessageForErrorCode(int errorCode);
            std::string getErrorMessageForErrorCode(DWORD errorCode);
            SystemError getSystemErrorForErrorCode(int errorCode, Paradigm paradigm);
            SystemError getSystemErrorForErrorCode(DWORD errorCode, Paradigm paradigm);
        } // namespace WindowsCommons
    } // namespace SystemErrors
} // namespace MF
#endif

#endif // MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_WINDOWSCOMMONS_HPP
