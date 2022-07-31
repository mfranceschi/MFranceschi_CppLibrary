//
// Created by MartinF on 31/07/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_WINDOWSCOMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_WINDOWSCOMMONS_HPP

#if MF_WINDOWS || 1

#    include <string>
#    include <system_error>

#    include "MF/LightWindows.hpp"

namespace MF
{
    namespace SystemErrors
    {
        namespace WindowsCommons
        {
            std::string getErrorMessageForErrorCode(int errorCode, bool localized);
            std::string getErrorMessageForErrorCode(DWORD errorCode, bool localized);
            std::system_error getSystemErrorForErrorCode(int errorCode);
            std::system_error getSystemErrorForErrorCode(DWORD errorCode);
        } // namespace WindowsCommons
    } // namespace SystemErrors
} // namespace MF
#endif

#endif // MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_WINDOWSCOMMONS_HPP
