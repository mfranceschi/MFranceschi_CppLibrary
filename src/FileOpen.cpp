//
// Created by mfran on 24/04/2020.
//

#include "FileOpen.hpp"

#if defined(_WIN32)
#   include "WindowsAPIHelper.hpp"
#else
#   include "UnixAPIHelper.hpp"
#endif

namespace File {
    const ReadFileData* Read(Filename_t filename)
    {
#ifdef _WIN32
        return Windows_OpenFile(filename);
#else
        return Unix_OpenFile(filename);
#endif
    }

    void Read_Close(const ReadFileData* content)
    {
#if defined(_WIN32)
        return Windows_CloseReadFileData(content);
#else
        return Unix_CloseReadFileData(content);
#endif
    }

    bool ReadToString(Filename_t filename, std::string& string) {
        auto fileContents = File::Read(filename);
        if (fileContents) {
            const char *contents = fileContents->contents;
            string.assign(contents, fileContents->size);
            File::Read_Close(fileContents);
            return true;
        } else {
            return false;
        }
    }
}