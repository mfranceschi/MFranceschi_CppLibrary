//
// Created by MartinF on 05/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP

#include "MF/Filesystem.hpp"

namespace MF {
    namespace File {
        bool osDeleteFile(File::Filename_t filename);

        bool osDeleteFileOrDirectory(File::Filename_t filename);

        bool osFileExists(File::Filename_t filename);

        bool osDirectoryExists(File::Filename_t filename);

        int osReadFileToBuffer(File::Filename_t filename, char *buffer, File::Filesize_t bufferSize);

        Filesize_t osGetFileSize(File::Filename_t filename);

        bool osCreateDirectory(File::Filename_t directoryName);

        SFilename_t osGetCWD();

        void osGetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t> &result);
    }
}

#endif //MFRANCESCHI_CPPLIBRARIES_FILESYSTEMOSHELPER_HPP
