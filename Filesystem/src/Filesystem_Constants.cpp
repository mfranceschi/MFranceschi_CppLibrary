//
// Created by MartinF on 16/10/2022.
//

#include "MF/Filesystem.hpp"

namespace MF
{
    namespace Filesystem
    {
#if MF_WINDOWS
        const Filename_t FILE_SEPARATOR = "\\";
        const WideFilename_t FILE_SEPARATOR_WIDE = L"\\";
        const Filename_t LINE_END = "\r\n";
        const WideFilename_t LINE_END_WIDE = L"\r\n";
#else
        const Filename_t FILE_SEPARATOR = "/";
        const WideFilename_t FILE_SEPARATOR_WIDE = L"/";
        const Filename_t LINE_END = "\n";
        const WideFilename_t LINE_END_WIDE = L"\n";
#endif
    } // namespace Filesystem
} // namespace MF