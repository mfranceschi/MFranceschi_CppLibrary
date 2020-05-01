//
// Created by mfran on 19/04/2020.
//

#if !defined(_WIN32)

#include <sys/stat.h>
#include <unistd.h>
#include "UnixAPIHelper.hpp"
#include <climits>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>

bool Unix_DeleteFile(File::Filename_t filename) {
    return !unlink(filename);
}

bool Unix_DeleteFileOrDirectory(File::Filename_t name) {
    return !remove(name);
}

bool Unix_FileExists(File::Filename_t filename) {
    struct stat s{};
    return !stat(filename, &s);
}

bool Unix_DirectoryExists(File::Filename_t directoryName) {
    struct stat s{};
    return !stat(directoryName, &s) & S_ISDIR(s.st_mode);
}

File::Filesize_t Unix_GetFileSize(File::Filename_t filename) {
    struct stat t{};
    if (stat(filename, &t)) return 0;
    return static_cast<File::Filesize_t>(t.st_size);
}

bool Unix_CreateDirectory(File::Filename_t directoryName) {
    return !mkdir(directoryName, S_IRWXU | S_IRWXG | S_IRWXO);
}

File::SFilename_t Unix_GetCurrentWorkingDirectory() {
    char* syscall_return;
#if defined(_GNU_SOURCE)
    syscall_return = get_current_dir_name();
#else
    syscall_return = static_cast<char *>(malloc(PATH_MAX));
    getcwd(syscall_return, PATH_MAX);
#endif
    File::SFilename_t to_return(syscall_return);
    free((void*)syscall_return);
    return to_return;
}

void Unix_GetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t>& result) {
    File::SFilename_t tempFilename;
    static File::Filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
    static File::Filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
    DIR * d;
    dirent * dir_entry;
    d = opendir(directoryName);
    if (d) {
        while ((dir_entry = readdir(d)) != nullptr) {
            tempFilename = dir_entry->d_name;
            if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                continue;
            } else if (File::IsDir((std::string(directoryName) + tempFilename).c_str())) {
                tempFilename.append(FILE_SEPARATOR);
            }
            result.emplace_back(tempFilename);
        }
        closedir(d);
    }
}

const Unix_ReadFileData* Unix_OpenFile(File::Filename_t filename) {
    auto rfd = new Unix_ReadFileData;

    if ((rfd->fd = open(filename, O_RDONLY)) == -1) {
        delete rfd;
        return nullptr;
    }

    struct stat st;
    if (fstat(rfd->fd, &st) == 0) {
        rfd->size = st.st_size;
    } else {
        delete rfd;
        return nullptr;
    }

    rfd->contents = (const char*)mmap(nullptr, rfd->size, PROT_READ, MAP_PRIVATE, rfd->fd, 0);
    if (rfd->contents == MAP_FAILED) {
        delete rfd;
        return nullptr;
    }

    return rfd;
}

void Unix_CloseReadFileData(const Unix_ReadFileData* readFileData)  {
    close(readFileData->fd);
    delete readFileData;
}

void Unix_CloseReadFileData(const File::ReadFileData* readFileData) {
    (void)(readFileData); // to avoid "unused parameter warning
}

int Unix_ReadFileToBuffer(File::Filename_t filename, char* buffer, File::Filesize_t bufferSize) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    int bytesRead = read(fd, buffer, bufferSize);
    close(fd);
    return bytesRead;
}

#endif // no _WIN32