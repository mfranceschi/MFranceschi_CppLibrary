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
#    include <functional>

#    include "FilesystemOSHelper.hpp"

template <typename Int_t>
static constexpr bool successOnZero(Int_t value) {
    return value == Int_t(0);
}

namespace MF
{
    namespace Filesystem
    {
        bool osDeleteFile(Filename_t filename) {
            return successOnZero(unlink(filename));
        }

        bool osDeleteFileOrDirectory(Filename_t name) {
            return successOnZero(remove(name));
        }

        bool osFileExists(Filename_t filename) {
            struct stat s {};
            return successOnZero(stat(filename, &s));
        }

        bool osDirectoryExists(Filename_t filename) {
            struct stat s {};
            return successOnZero(stat(filename, &s)) && S_ISDIR(s.st_mode);
        }

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize) {
            int fileDescriptor = open(filename, O_RDONLY);
            if (fileDescriptor == -1) {
                return -1;
            }

            int bytesRead = read(fileDescriptor, buffer, bufferSize);
            close(fileDescriptor);
            return bytesRead;
        }

        Filesize_t osGetFileSize(Filename_t filename) {
            struct stat t {};
            if (stat(filename, &t) == 0) {
                return 0;
            }
            return static_cast<Filesize_t>(t.st_size);
        }

        bool osCreateDirectory(Filename_t directoryName) {
            return successOnZero(mkdir(directoryName, S_IRWXU | S_IRWXG | S_IRWXO));
        }

        SFilename_t osGetCWD() {
            using deleter_t = std::function<void(char *)>;
            static const auto deleter = [](char *pointer) {
                std::free(pointer);
            };
            std::unique_ptr<char[], deleter_t> syscallReturn(nullptr, deleter);

#    if defined(_GNU_SOURCE)
            syscallReturn.reset(get_current_dir_name());
#    else
            syscallReturn.reset(static_cast<char *>(malloc(PATH_MAX)));
            getcwd(syscallReturn.get(), PATH_MAX);
#    endif
            return syscallReturn.get();
        }

        void osGetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result) {
            SFilename_t tempFilename;
            static Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
            static Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
            DIR *dirStream = nullptr;
            dirent *dir_entry = nullptr;
            dirStream = opendir(directoryName);

            if (dirStream != nullptr) {
                while ((dir_entry = readdir(dirStream)) != nullptr) {
                    tempFilename = dir_entry->d_name;
                    if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                        continue;
                    }

                    if (IsDir((std::string(directoryName) + tempFilename).c_str())) {
                        tempFilename.append(FILE_SEPARATOR);
                    }
                    result.emplace_back(tempFilename);
                }
                closedir(dirStream);
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
