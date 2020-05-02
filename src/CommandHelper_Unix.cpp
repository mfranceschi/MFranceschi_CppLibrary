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

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

FD_t ProcessInputStream_None::getFD() const {
    return STDIN_FILENO;
}

void ProcessInputStream_String::beforeStart() {
    FD_t fd[2];
    pipe(fd);
    readPipeFD = fd[0];
    writeToPipeFD = fd[1];
    fcntl(writeToPipeFD, F_SETFD, FD_CLOEXEC);
}

void ProcessInputStream_String::afterStart() {
    std::string temp;
    temp.reserve(500);
    for (std::size_t i = 0; i < inputString.length(); i += 500) {
        temp = inputString.substr(i, 500);
        write(writeToPipeFD, temp.c_str(), std::min(temp.length(), static_cast<std::size_t>(500)));
    }
}

void ProcessInputStream_String::afterStop() {
    close(readPipeFD);
    close(writeToPipeFD);
}

FD_t ProcessInputStream_String::getFD() const {
    return readPipeFD;
}

void ProcessInputStream_FromFile::beforeStart() {
    fileFD = open(filename.c_str(), O_RDONLY);
}

void ProcessInputStream_FromFile::afterStop() {
    close(fileFD);
}

FD_t ProcessInputStream_FromFile::getFD() const {
    return fileFD;
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////
FD_t ProcessOutputStream_Keep::getFD() const {
    return STDOUT_FILENO;
}

FD_t ProcessErrorStream_Keep::getFD() const {
    return STDERR_FILENO;
}

void ProcessOutputStream_Kill::beforeStart() {
    fileFD = open("/dev/null", O_WRONLY);
}

void ProcessOutputStream_Kill::afterStop() {
    close(fileFD);
}

FD_t ProcessOutputStream_Kill::getFD() const {
    return fileFD;
}

void ProcessOutputStream_Export::beforeStart() {
    int flags = O_WRONLY;
    if (APPEND) {
        flags |= O_APPEND;
    }
    mode_t mode = S_IRWXO;
    fileFD = open(filename.c_str(), flags, mode);
}

void ProcessOutputStream_Export::afterStop() {
    close(fileFD);
}

FD_t ProcessOutputStream_Export::getFD() const {
    return fileFD;
}

void ProcessOutputStream_Retrieve::beforeStart() {
    FD_t fd[2];
    pipe(fd);
    readPipeFD = fd[0];
    writeToPipeFD = fd[1];
    fcntl(readPipeFD, F_SETFD, FD_CLOEXEC);
    fcntl(readPipeFD, F_SETFL, O_NONBLOCK);
    fcntl(writeToPipeFD, F_SETFL, O_NONBLOCK);
}

void ProcessOutputStream_Retrieve::beforeStop() {}

void ProcessOutputStream_Retrieve::afterStop() {
    char chBuf[BUFFER_LENGTH] = {0};
    bool bContinue = true;
    ssize_t nbRead;

    printf("Errno before read: %d\n", errno);

    while (bContinue) {
        nbRead = read(readPipeFD, chBuf, BUFFER_LENGTH - 1);
        printf("Errno of read: %d\n", errno);

        if (nbRead > 0) {
            oss << chBuf;
        } else if (nbRead == -1) {
            bContinue = false;
        } else {
            bContinue = false;
        }
    }
    close(readPipeFD);
    close(writeToPipeFD);
}

std::string ProcessOutputStream_Retrieve::retrieveOutput() {
    return oss.str();
}

FD_t ProcessOutputStream_Retrieve::getFD() const {
    return writeToPipeFD;
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

void CommandRunner::internalStart() {

    forkResult = fork();

    if (forkResult == 0) {
        // Child process
        const char* file = executable->c_str();
        const char** const argv = new const char* [arguments->size() + 2];
        {
            argv[0] = file;
            for (std::size_t i = 0; i < arguments->size(); i++) {
                argv[i + 1] = (*arguments)[i].c_str();
            }
            argv[arguments->size() + 1] = static_cast<char *>(nullptr);
        }

        dup2(processInputStream->getFD(),  STDIN_FILENO);
        dup2(processOutputStream->getFD(), STDOUT_FILENO);
        dup2(processErrorStream->getFD(),  STDERR_FILENO);
        write(STDOUT_FILENO, "coucou", 6); // TODO remove

        /*
         * Using "Exec VP" because I want the shell to find the executable according to usual rules,
         * and I cannot use variadic functions because the number of arguments is only known at runtime.
         */
        int execvpResult = execvp(file, const_cast<char *const *>(argv));
        if (execvpResult == -1) {
            // TODO handle error
            exit(44);
        }
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
    waitpid(forkResult, &status, 0);
    std::printf("Termination signal: %d\n", WTERMSIG(status));

    return WIFEXITED(status) ? WEXITSTATUS(status) : 44; // TODO handle unfinished process
}

void CommandRunner::internalOSCleanUp() {}
#endif
