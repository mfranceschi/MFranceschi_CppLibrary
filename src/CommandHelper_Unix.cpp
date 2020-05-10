//
// Created by mfran on 01/05/2020.
//

#if !defined(_WIN32)

#include "UnixAPIHelper.hpp"
#include "CommandHelper.hpp"
#include <csignal>
#include <sys/wait.h>
#include <fcntl.h>
#include <cassert>

static constexpr unsigned int BUFFER_LENGTH = 4096;

static inline int makePipe(int& read, int& write) {
    int fd[2];
    int pipeResult = pipe(fd);
    read = fd[0];
    write = fd[1];
    return pipeResult;
}

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

void ProcessStream::closeOnFork() {}

StreamItem ProcessInputStream_None::getStreamItem() const {
    return STDIN_FILENO;
}

void ProcessInputStream_String::beforeStart() {
    makePipe(readStream, writeToStream);
    fcntl(writeToStream, F_SETFD, FD_CLOEXEC);
}

void ProcessInputStream_String::afterStart() {
    std::string temp;
    temp.reserve(500);
    for (std::size_t i = 0; i < inputString.length(); i += 500) {
        temp = inputString.substr(i, 500);
        write(writeToStream, temp.c_str(), std::min(temp.length(), static_cast<std::size_t>(500)));
    }
}

void ProcessInputStream_String::afterStop() {
    close(readStream);
    close(writeToStream);
}

StreamItem ProcessInputStream_String::getStreamItem() const {
    return readStream;
}

void ProcessInputStream_String::closeOnFork() {
    close(readStream);
    close(writeToStream);
}

void ProcessInputStream_FromFile::beforeStart() {
    fileStream = open(filename.c_str(), O_RDONLY);
}

void ProcessInputStream_FromFile::afterStop() {
    close(fileStream);
}

StreamItem ProcessInputStream_FromFile::getStreamItem() const {
    return fileStream;
}

void ProcessInputStream_FromFile::closeOnFork() {
    close(fileStream);
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////
StreamItem ProcessOutputStream_Keep::getStreamItem() const {
    return STDOUT_FILENO;
}

StreamItem ProcessErrorStream_Keep::getStreamItem() const {
    return STDERR_FILENO;
}

File::Filename_t ProcessOutputStream_Kill::KILL_FILENAME = "/dev/null";

void ProcessOutputStream_Kill::beforeStart() {
    fileStream = open(filename.c_str(), O_WRONLY);
}

void ProcessOutputStream_Export::beforeStart() {
    int flags = O_WRONLY;
    if (APPEND) {
        flags |= O_APPEND;
    }
    mode_t mode = S_IRWXO;
    fileStream = open(filename.c_str(), flags, mode);
}

void ProcessOutputStream_Export::afterStop() {
    close(fileStream);
}

StreamItem ProcessOutputStream_Export::getStreamItem() const {
    return fileStream;
}

void ProcessOutputStream_Export::closeOnFork() {
    close(fileStream);
}

void ProcessOutputStream_Retrieve::beforeStart() {
    makePipe(readStream, writeStream);
    fcntl(readStream, F_SETFD, FD_CLOEXEC);
    fcntl(readStream, F_SETFL, O_NONBLOCK);
    fcntl(writeStream, F_SETFL, O_NONBLOCK);
}

void ProcessOutputStream_Retrieve::beforeStop() {}

void ProcessOutputStream_Retrieve::afterStop() {
    char chBuf[BUFFER_LENGTH] = {0};
    bool bContinue = true;
    ssize_t nbRead;

    printf("Errno before read: %d\n", errno);

    while (bContinue) {
        nbRead = read(readStream, chBuf, BUFFER_LENGTH - 1);
        printf("Errno of read: %d\n", errno);

        if (nbRead > 0) {
            oss << chBuf;
        } else { // TODO handle case -1
            bContinue = false;
        }
    }
    close(readStream);
    close(writeStream);
}

std::string ProcessOutputStream_Retrieve::retrieveOutput() {
    return oss.str();
}

StreamItem ProcessOutputStream_Retrieve::getStreamItem() const {
    return writeStream;
}

void ProcessOutputStream_Retrieve::closeOnFork() {
    //close(readStream);
    close(writeStream);
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

void CommandRunner::internalStart() {

    childProcessItem = fork();

    if (childProcessItem == 0) {
        // Child process
        const char* file = executable->c_str();
        const char** const argv = new const char* [arguments->size() + 2];
        {
            argv[0] = file;
            for (std::size_t i = 0; i < arguments->size(); i++) {
                const std::string& current = (*arguments)[i];
                if (current[0] == '\"' && current[current.size() - 1] == '\"') {
                    argv[i + 1] = current.substr(1, current.size() - 2).c_str();
                } else {
                    argv[i + 1] = current.c_str();
                }
            }
            argv[arguments->size() + 1] = static_cast<char *>(nullptr);
        }

        dup2(processInputStream->getStreamItem(),  STDIN_FILENO);
        dup2(processOutputStream->getStreamItem(), STDOUT_FILENO);
        dup2(processErrorStream->getStreamItem(),  STDERR_FILENO);
        /*processInputStream->closeOnFork();
        processOutputStream->closeOnFork();
        processErrorStream->closeOnFork();*/
        //write(STDOUT_FILENO, "coucou", 6); // TODO remove

        /*
         * Using "Exec VP" because I want the shell to find the executable according to usual rules,
         * and I cannot use variadic functions because the number of arguments is only known at runtime.
         */
        execvp(file, const_cast<char *const *>(argv));
        // TODO handle error
        exit(44);
    } else {
        // Parent process

    }
}

void CommandRunner::internalStop() {
    //kill(forkResult, SIGTERM);
    std::printf("Errno at line %d: %d\n", __LINE__, errno);
}

int CommandRunner::internalGetStatusCode() {
    int status;
    waitpid(childProcessItem, &status, 0);
    if (WIFSIGNALED(status)) {
        std::printf("Termination signal: %d\n", WTERMSIG(status));
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : 55; // TODO handle unfinished process
}

void CommandRunner::internalOSCleanUp() {}

#endif
