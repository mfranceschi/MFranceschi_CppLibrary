//
// Created by Utilisateur on 22/03/2022.
//

//
// Created by MartinF on 05/03/2022.
//

#if MF_UNIX

#    include <dirent.h>
#    include <fcntl.h>
#    include <sys/mman.h>
#    include <sys/stat.h>
#    include <unistd.h>

#    include <cassert>

#    include "FilesystemOSHelper.hpp"

namespace MF
{
    namespace Filesystem
    {
        bool osDeleteFile(Filename_t filename) {
            return !unlink(filename);
        }

        bool osDeleteFileOrDirectory(Filename_t name) {
            return !remove(name);
        }

        bool osFileExists(Filename_t filename) {
            struct stat s {};
            return !stat(filename, &s);
        }

        bool osDirectoryExists(Filename_t filename) {
            struct stat s {};
            return !stat(filename, &s) & S_ISDIR(s.st_mode);
        }

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize) {
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                return -1;
            }

            int bytesRead = read(fd, buffer, bufferSize);
            close(fd);
            return bytesRead;
        }

        Filesize_t osGetFileSize(Filename_t filename) {
            struct stat t {};
            if (stat(filename, &t)) return 0;
            return static_cast<Filesize_t>(t.st_size);
        }

        bool osCreateDirectory(Filename_t directoryName) {
            !mkdir(directoryName, S_IRWXU | S_IRWXG | S_IRWXO);
        }

        SFilename_t osGetCWD() {
            char *syscall_return;
#    if defined(_GNU_SOURCE)
            syscall_return = get_current_dir_name();
#    else
            syscall_return = static_cast<char *>(malloc(PATH_MAX));
            getcwd(syscall_return, PATH_MAX);
#    endif
            SFilename_t to_return(syscall_return);
            free((void *)syscall_return);
            return to_return;
        }

        void osGetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result) {
            SFilename_t tempFilename;
            static Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
            static Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
            DIR *d;
            dirent *dir_entry;
            d = opendir(directoryName);
            if (d) {
                while ((dir_entry = readdir(d)) != nullptr) {
                    tempFilename = dir_entry->d_name;
                    if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                        continue;
                    } else if (IsDir((std::string(directoryName) + tempFilename).c_str())) {
                        tempFilename.append(FILE_SEPARATOR);
                    }
                    result.emplace_back(tempFilename);
                }
                closedir(d);
            }
        }

        struct Unix_ReadFileData : public ReadFileData {
            int fd = -1;
        };
        using osReadFileData_t = Unix_ReadFileData;

        std::unique_ptr<const ReadFileData> osOpenFile(Filename_t filename) {
            auto rfd = std::make_unique<Unix_ReadFileData>();

            if ((rfd->fd = open(filename, O_RDONLY)) == -1) {
                return nullptr;
            }

            struct stat st {};
            if (fstat(rfd->fd, &st) == 0) {
                rfd->size = st.st_size;
            } else {
                return nullptr;
            }

            rfd->contents =
                (const char *)mmap(nullptr, rfd->size, PROT_READ, MAP_PRIVATE, rfd->fd, 0);
            if (rfd->contents == MAP_FAILED) {
                return nullptr;
            }

            return rfd;
        }

        void osCloseReadFileData(const ReadFileData *readFileData1) {
            const auto *readFileData = dynamic_cast<const osReadFileData_t *>(readFileData1);
            assert(readFileData);

            close(readFileData->fd);
            delete readFileData;
        }

    } // namespace Filesystem
} // namespace MF

#endif
