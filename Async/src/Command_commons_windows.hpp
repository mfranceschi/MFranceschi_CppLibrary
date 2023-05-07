//
// Created by MartinF on 08/05/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_WINDOWS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_WINDOWS_HPP

#if MF_WINDOWS
#    include "Command_commons.hpp"

namespace MF
{
    namespace Command
    {
        StreamItem openFileToRead(const WideFilename_t &filename);
        StreamItem openFileToWrite(const WideFilename_t &filename);

        std::vector<char> makeCommandLine(
            const Filename_t *executable, const std::vector<std::string> *arguments);
        std::vector<wchar_t> makeCommandLine(
            const WideFilename_t *executable, const std::vector<std::wstring> *arguments);

        void populateStartupInfo(const CommandCall &commandCall, STARTUPINFOA &startupinfo);
        void populateStartupInfo(const WideCommandCall &commandCall, STARTUPINFOW &startupinfo);

        SECURITY_ATTRIBUTES &getInheritableSecAttr();

        /**
         * If the handle is invalid, do nothing.
         * Otherwise calls CloseHandle then assigns it to invalid handle.
         */
        void closeH(HANDLE &handle);
    } // namespace Command
} // namespace MF
#endif

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_WINDOWS_HPP
