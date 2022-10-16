//
// Created by MartinF on 05/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP

#include "MF/Filesystem.hpp"

namespace MF
{
    namespace Filesystem
    {

        void osReadFileToBuffer(const Filename_t &filename, char *buffer, Filesize_t bufferSize);

        void osGetDirectoryContents(
            const Filename_t &directoryName, std::vector<Filename_t> &result);

#if MF_WINDOWS
        void osReadFileToBuffer(
            const WideFilename_t &filename, char *buffer, Filesize_t bufferSize);

        void osGetDirectoryContents(
            const WideFilename_t &directoryName, std::vector<WideFilename_t> &result);
#endif
    } // namespace Filesystem
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP
