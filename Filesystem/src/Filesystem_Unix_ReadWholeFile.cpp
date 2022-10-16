//
// Created by MartinF on 16/10/2022.
//

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>

#include "MF/Filesystem.hpp"
#include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Filesystem
    {
        static const auto fdCloseProcedure = [](const int *theFd) {
            close(*theFd);
        };
        static auto makeAutoClosingFd(int fd) {
            return std::unique_ptr<decltype(fd), decltype(fdCloseProcedure)>(&fd, fdCloseProcedure);
        }

        struct Unix_ReadFileData : public WholeFileData {
            Unix_ReadFileData(void *content, Filesize_t filesize)
                : WholeFileData(static_cast<const char *>(content), filesize) {
            }

            ~Unix_ReadFileData() {
                assert(munmap((void *)getContent(), getSize()) == 0);
            }
        };

        std::unique_ptr<const WholeFileData> readWholeFile(const Filename_t &filename) {
            const int fileDescriptor = open(filename.c_str(), O_RDONLY);
            MF::SystemErrors::Errno::throwCurrentSystemErrorIf(fileDescriptor == -1);
            auto fdAutoClose = makeAutoClosingFd(fileDescriptor);

            struct stat statOfFile {};
            MF::SystemErrors::Errno::throwCurrentSystemErrorIf(
                fstat(fileDescriptor, &statOfFile) != 0);
            Filesize_t filesize = statOfFile.st_size;

            void *mmapResult = mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
            MF::SystemErrors::Errno::throwCurrentSystemErrorIf(mmapResult == MAP_FAILED);

            return std::make_unique<const Unix_ReadFileData>(mmapResult, filesize);
        }
    } // namespace Filesystem
} // namespace MF
