//
// Created by mfran on 01/05/2020.
//

#if !defined(_WIN32)

#include "UnixAPIHelper.hpp"
#include "CommandHelper.hpp"
#include <csignal>
#include <sys/wait.h>

static constexpr unsigned int BUFFER_LENGTH = 4096;

// ///////////////////////////////////////////////////////////////
// /////////////////////// INPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////

FileDescriptor_t ProcessInputStream_None::getFD() const {
    return STDIN_FILENO;
}

#if 0
void ProcessInputStream_String::beforeStart() {
    SECURITY_ATTRIBUTES securityAttributes {
            sizeof(SECURITY_ATTRIBUTES),
            nullptr,
            true
    };
    CreatePipe(&readPipeHandle, &writeToPipeHandle, &securityAttributes, BUFFER_LENGTH);
    Windows_MakeHandleInheritable(readPipeHandle);
}

void ProcessInputStream_String::afterStart() {
    DWORD lpWritten;
    std::string temp;
    temp.reserve(999);
    for (std::size_t i = 0; i < inputString.length(); i += 999) {
        temp = inputString.substr(i, 999);
        WriteFile(writeToPipeHandle, temp.c_str(), 999, &lpWritten, nullptr);
    }
}

void ProcessInputStream_String::afterStop() {
    CloseHandle(writeToPipeHandle);
    CloseHandle(readPipeHandle);
}

HANDLE ProcessInputStream_String::getHandle() const {
    return readPipeHandle;
}

void ProcessInputStream_FromFile::beforeStart() {
    fileHandle = CreateFile(
            filename.c_str(),
            FILE_GENERIC_READ,
            FILE_SHARE_READ,
            &securityAttributesForInheritableHandles,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
}

void ProcessInputStream_FromFile::afterStop() {
    CloseHandle(fileHandle);
}

HANDLE ProcessInputStream_FromFile::getHandle() const {
    return fileHandle;
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// OUTPUT STREAMS /////////////////////////
// ///////////////////////////////////////////////////////////////
#endif
FileDescriptor_t ProcessOutputStream_Keep::getFD() const {
    return STDOUT_FILENO;
}

FileDescriptor_t ProcessErrorStream_Keep::getFD() const {
    return STDERR_FILENO;
}
#if 0
void ProcessOutputStream_Kill::beforeStart() {
    nulHandle = CreateFile(
            TEXT("NUL"),
            FILE_GENERIC_WRITE,
            FILE_SHARE_READ,
            &securityAttributesForInheritableHandles,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_DEVICE,
            nullptr);
}

void ProcessOutputStream_Kill::afterStop() {
    CloseHandle(nulHandle);
}

HANDLE ProcessOutputStream_Kill::getHandle() const {
    return nulHandle;
}

void ProcessOutputStream_Export::beforeStart() {
    fileHandle = CreateFile(
            filename.c_str(),
            FILE_GENERIC_WRITE,
            FILE_SHARE_READ,
            &securityAttributesForInheritableHandles,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

    if (APPEND) {
        SetFilePointer(fileHandle, 0, nullptr, FILE_END);
    }
}

void ProcessOutputStream_Export::afterStop() {
    CloseHandle(fileHandle);
}

HANDLE ProcessOutputStream_Export::getHandle() const {
    return fileHandle;
}

void ProcessOutputStream_Retrieve::beforeStart() {
    DWORD dwMode = PIPE_NOWAIT;
    SECURITY_ATTRIBUTES securityAttributes {
        sizeof(SECURITY_ATTRIBUTES),
        nullptr,
        true
    };
    CreatePipe(&readPipeHandle, &writeToPipeHandle, &securityAttributes, BUFFER_LENGTH);
    Windows_MakeHandleInheritable(writeToPipeHandle);
    SetNamedPipeHandleState(readPipeHandle, &dwMode, nullptr, nullptr);
}

void ProcessOutputStream_Retrieve::beforeStop() {
    DWORD dwRead;
    CHAR chBuf[BUFFER_LENGTH];

    if (ReadFile(readPipeHandle, chBuf, BUFFER_LENGTH - 1, &dwRead, nullptr) || (dwRead != 0)) {
        chBuf[dwRead] = '\0';
        oss << chBuf;
    }
}

void ProcessOutputStream_Retrieve::afterStop() {
    DWORD dwRead;
    CHAR chBuf[BUFFER_LENGTH];

    while (ReadFile(readPipeHandle, chBuf, BUFFER_LENGTH - 1, &dwRead, nullptr) || (dwRead != 0)) {
        chBuf[dwRead] = '\0';
        oss << chBuf;
    }
    CloseHandle(writeToPipeHandle);
    CloseHandle(readPipeHandle);
}

std::string ProcessOutputStream_Retrieve::retrieveOutput() {
    return oss.str();
}

HANDLE ProcessOutputStream_Retrieve::getHandle() const {
    return writeToPipeHandle;
}

// ///////////////////////////////////////////////////////////////
// ////////////////////// COMMAND RUNNER /////////////////////////
// ///////////////////////////////////////////////////////////////

#endif

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

        dup2(processInputStream->getFD(), STDIN_FILENO);
        dup2(processOutputStream->getFD(), STDOUT_FILENO);
        dup2(processErrorStream->getFD(), STDERR_FILENO);

        /*
         * Using "Exec VP" because I want the shell to find the executable according to usual rules,
         * and I cannot use variadic functions because the number of arguments is only known at runtime.
         */
        int execvpResult = execvp(file, const_cast<char *const *>(argv));
        if (execvpResult == -1) {
            // TODO handle error
        }
    } else {
        // Parent process

    }
}

void CommandRunner::internalStop() {
    kill(forkResult, SIGTERM);
}

int CommandRunner::internalGetStatusCode() {
    int status;
    waitpid(forkResult, &status, WNOHANG);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 44; // TODO handle unfinished process
}

void CommandRunner::internalOSCleanUp() {}
#endif
