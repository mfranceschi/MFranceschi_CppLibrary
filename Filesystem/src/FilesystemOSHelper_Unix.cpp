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

/** Helper method. Returns true if value is zero. */
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

        bool osDeleteDirectory(Filename_t filename) {
            return successOnZero(rmdir(filename));
        }

        bool osFileExists(Filename_t filename) {
            struct stat statOfFile {};
            return successOnZero(stat(filename, &statOfFile));
        }

        bool osDirectoryExists(Filename_t filename) {
            struct stat statOfFile {};
            return successOnZero(stat(filename, &statOfFile)) && S_ISDIR(statOfFile.st_mode);
        }

        int osReadFileToBuffer(Filename_t filename, char *buffer, Filesize_t bufferSize) {
            int fileDescriptor = open(filename, O_RDONLY);
            if (fileDescriptor == -1) {
                return -1;
            }

            ssize_t bytesRead = read(fileDescriptor, buffer, bufferSize);
            close(fileDescriptor);
            return bytesRead;
        }

        Filesize_t osGetFileSize(Filename_t filename) {
            struct stat statOfFile {};
            if (stat(filename, &statOfFile) == 0) {
                return 0;
            }
            return static_cast<Filesize_t>(statOfFile.st_size);
        }

        bool osCreateDirectory(Filename_t directoryName) {
            return successOnZero(mkdir(directoryName, S_IRWXU | S_IRWXG | S_IRWXO));
        }

        SFilename_t osGetCWD() {
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

        void osGetDirectoryContents(Filename_t directoryName, std::vector<SFilename_t> &result) {
            SFilename_t tempFilename;
            static Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
            static Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
            std::unique_ptr<DIR, decltype(&closedir)> dirStream(opendir(directoryName), closedir);
            dirent *dir_entry = nullptr;

            if (dirStream) {
                while ((dir_entry = readdir(dirStream.get())) != nullptr) {
                    tempFilename = dir_entry->d_name;
                    if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                        continue;
                    }

                    if (IsDir((std::string(directoryName) + tempFilename).c_str())) {
                        tempFilename.append(FILE_SEPARATOR);
                    }
                    result.emplace_back(tempFilename);
                }
                // closedir(dirStream);
            }
        }

        struct Unix_ReadFileData : public WholeFileData {
            ~Unix_ReadFileData() {
                assert(munmap((void *)contents, size) == 0);
            }
        };
        using osReadFileData_t = Unix_ReadFileData;

        std::unique_ptr<const WholeFileData> osReadWholeFile(Filename_t filename) {
            const int fileDescriptor = open(filename, O_RDONLY);
            if (fileDescriptor == -1) {
                return nullptr;
            }

            // Declare it as a unique pointer
            // to take advantage of the auto-closing resource feature.
            static const auto fdCloseProcedure = [](const int *theFd) {
                close(*theFd);
            };
            std::unique_ptr<decltype(fileDescriptor), decltype(fdCloseProcedure)> fdAutoClose(
                &fileDescriptor, fdCloseProcedure);

            auto rfd = std::make_unique<Unix_ReadFileData>();

            struct stat statOfFile {};
            if (fstat(fileDescriptor, &statOfFile) != 0) {
                return nullptr;
            }
            rfd->size = statOfFile.st_size;

            void *mmapResult = mmap(nullptr, rfd->size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
            if (mmapResult == MAP_FAILED) {
                return nullptr;
            }
            rfd->contents = static_cast<const char *>(mmapResult);

            return rfd;
        }

    } // namespace Filesystem
} // namespace MF

#endif
