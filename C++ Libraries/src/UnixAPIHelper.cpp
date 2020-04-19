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

bool _UnixDeleteFile(File::Filename_t filename) {
    return !unlink(filename);
}

bool _UnixDeleteFileOrDirectory(File::Filename_t name) {
    return !remove(name);
}

bool _UnixFileExists(File::Filename_t filename) {
    struct stat s{};
    return !stat(filename, &s);
}

bool _UnixDirectoryExists(File::Filename_t directoryName) {
    struct stat s{};
    return !stat(directoryName, &s) & S_ISDIR(s.st_mode);
}

File::Filesize_t _UnixGetFileSize(File::Filename_t filename) {
    struct stat t{};
    if (stat(filename, &t)) return 0;
    return static_cast<File::Filesize_t>(t.st_size);
}

bool _UnixCreateDirectory(File::Filename_t directoryName) {
    return !mkdir(directoryName, S_IRWXU | S_IRWXG | S_IRWXO);
}

File::SFilename_t _UnixGetCurrentWorkingDirectory() {
    char* syscall_return;
#if !defined(_GNU_SOURCE)
    syscall_return = get_current_dir_name();
#else
    syscall_return = static_cast<char *>(malloc(PATH_MAX));
    getcwd(syscall_return, PATH_MAX);
#endif
    File::SFilename_t to_return(syscall_return);
    free((void*)syscall_return);
    return to_return;
}

void _UnixGetDirectoryContents(File::Filename_t directoryName, std::vector<File::SFilename_t>& result) {
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

const _UnixReadFileData* _UnixOpenFile(File::Filename_t filename) {
    auto rfd = new _UnixReadFileData;

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

void _UnixCloseReadFileData(const _UnixReadFileData* readFileData)  {
    close(readFileData->fd);
    delete readFileData;
}

void _UnixCloseReadFileData(const File::ReadFileData* readFileData) {
    (void)(readFileData); // to avoid "unused parameter warning
}

int _UnixReadFileToBuffer(File::Filename_t filename, char* buffer, File::Filesize_t bufferSize) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    int bytesRead = read(fd, buffer, bufferSize);
    close(fd);
    return bytesRead;
}

#endif // no _WIN32