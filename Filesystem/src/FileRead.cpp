//
// Created by mfran on 24/04/2020.
//

#include "MF/Filesystem.hpp"

#include "FilesystemOSHelper.hpp"

namespace MF {
    namespace Filesystem {
        std::unique_ptr<const ReadFileData> Read(Filename_t filename) {
            return osOpenFile(filename);
        }

        bool ReadToString(Filename_t filename, std::string &string) {
            auto fileContents = Read(filename);
            if (fileContents) {
                auto contents = fileContents->contents;
                string.assign(contents, fileContents->size);
                return true;
            } else {
                return false;
            }
        }
    }
}
