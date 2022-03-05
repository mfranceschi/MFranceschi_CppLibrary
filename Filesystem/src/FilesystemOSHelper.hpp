//
// Created by MartinF on 05/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP

#include "MF/Filesystem.hpp"

namespace MF {
    namespace Filesystem {
        bool osDeleteFile(Filename_t filename);

        bool osDeleteFileOrDirectory(Filename_t filename);

        bool osFileExists(Filename_t filename);

        bool osDirectoryExists(Filename_t filename);

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize);

        Filesize_t osGetFileSize(Filename_t filename);

        bool osCreateDirectory(Filename_t directoryName);

        SFilename_t osGetCWD();

        void osGetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result);

        /**
         * Opens the given file and returns a pointer to a ReadFileData structure.
         * @param filename Name of the file to open.
         * @return A new structure, or nullptr if anything failed.
         */
        const ReadFileData *osOpenFile(Filename_t filename);

        /**
         * Releases the memory associated with the file opened there.
         * @param readFileData A Windows ReadFileData to close ; its memory will also be freed.
         */
        void osCloseReadFileData(const ReadFileData *readFileData);
    }
}

#endif //MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP
