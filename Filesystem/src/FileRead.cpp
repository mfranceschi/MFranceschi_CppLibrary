//
// Created by mfran on 24/04/2020.
//

#include "FilesystemOSHelper.hpp"
#include "MF/Filesystem.hpp"

namespace MF
{
    namespace Filesystem
    {
        std::unique_ptr<const ReadFileData> Read(Filename_t filename) {
            return osOpenFile(filename);
        }

        bool ReadToString(Filename_t filename, std::string &string) {
            auto fileContents = Read(filename);
            if (fileContents) {
                string.assign(fileContents->contents, fileContents->size);
                return true;
            }

            return false;
        }
    } // namespace Filesystem
} // namespace MF
