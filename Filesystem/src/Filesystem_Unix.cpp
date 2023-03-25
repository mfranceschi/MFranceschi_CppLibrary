//
// Created by MartinF on 16/10/2022.
//

#if MF_UNIX
#    include <sys/stat.h>
#    include <unistd.h>

#    include "MF/Filesystem.hpp"
#    include "MF/SystemErrors.hpp"

using namespace MF::SystemErrors;

namespace MF
{
    namespace Filesystem
    {
        void deleteFile(const Filename_t &filename) {
            int result = unlink(filename.c_str());
            Errno::throwCurrentSystemErrorIf(result != 0);
        }

        void deleteDirectory(const Filename_t &filename) {
            int result = rmdir(filename.c_str());
            Errno::throwCurrentSystemErrorIf(result != 0);
        }

        bool isFile(const Filename_t &filename) {
            struct stat statOfFile {};
            if (stat(filename.c_str(), &statOfFile) != 0) {
                return false;
            }
            return S_ISREG(statOfFile.st_mode);
        }

        bool isDir(const Filename_t &filename) {
            struct stat statOfFile {};
            if (stat(filename.c_str(), &statOfFile) != 0) {
                return false;
            }

            return S_ISDIR(statOfFile.st_mode);
        }

        Filesize_t getFileSize(const Filename_t &filename) {
            struct stat statOfFile {};
            Errno::throwCurrentSystemErrorIf(stat(filename.c_str(), &statOfFile) != 0);

            return static_cast<Filesize_t>(statOfFile.st_size);
        }

        void createDirectory(const Filename_t &directoryName) {
            int result = mkdir(directoryName.c_str(), S_IRWXU);
            Errno::throwCurrentSystemErrorIf(result != 0);
        }

        Filename_t getCWD() {
            static const auto deleter = [](char *pointer) {
                std::free(pointer);
            };
            std::unique_ptr<char, decltype(deleter)> syscallReturn(nullptr, deleter);

#    if defined(_GNU_SOURCE)
            syscallReturn.reset(get_current_dir_name());
#    else
            syscallReturn.reset(static_cast<char *>(malloc(PATH_MAX)));
            getcwd(syscallReturn.get(), PATH_MAX);
#    endif
            return syscallReturn.get();
        }
    } // namespace Filesystem
} // namespace MF
#endif
