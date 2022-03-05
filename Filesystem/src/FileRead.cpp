//
// Created by mfran on 24/04/2020.
//

#include "MF/Filesystem.hpp"

#include "FilesystemOSHelper.hpp"

namespace MF {
    namespace Filesystem {
        const ReadFileData *Read(Filename_t filename) {
            return osOpenFile(filename);
        }

        void Read_Close(const ReadFileData *content) {
            return osCloseReadFileData(content);
        }

        bool ReadToString(Filename_t filename, std::string &string) {
            auto fileContents = Read(filename);
            if (fileContents) {
                const char *contents = fileContents->contents;
                string.assign(contents, fileContents->size);
                Read_Close(fileContents);
                return true;
            } else {
                return false;
            }
        }
    }
}
