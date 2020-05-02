//
// Created by mfran on 08/12/2019.
//

#include <iostream>
#include <MFranceschi_CppLibrary.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <sys/wait.h>

using namespace std;

int main() {
    cout << "Current working dir: " << File::GetCWD().c_str() << endl;

    /*
    for (int i=0; i < 0; i++) {
        CommandCall call;
        call.executable = R"(C:\cygwin64\bin\pwd.exe)";
        call.outputChoice = OutputChoice::RETRIEVE;
        CommandReturn cr;
        cr.returnCode = 44;
        Command(call, cr);
    }
    Toolbox::PressAnyKeyToContinue();
    */

    const char* executable = "pwd";
    int fd[2];
    pipe(fd);
    fcntl(fd[0], F_SETFD, FD_CLOEXEC);
    fcntl(fd[0], F_SETFL, O_NONBLOCK);

    pid_t forkResult = fork();
    if ( ! forkResult) {
        // child
        dup2(fd[1], STDOUT_FILENO);

        /*
         * Using "Exec VP" because I want the shell to find the executable according to usual rules,
         * and I cannot use variadic functions because the number of arguments is only known at runtime.
         */
        if (execlp(executable, executable, nullptr)) {
            cout << "sniff i could not execl" << endl;
        }
    } else {
        // parent
        int status;
        if (waitpid(forkResult, &status, 0) == -1) {
            cout << "Errno for witpid: " << errno << endl;
            exit(1);
        }

        if ( ! WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            cout << "Status: " << status << endl;
            exit(1);
        }

        char buffer[100] = {0};
        cout << "Errno: " << errno << endl;

        int readResult = read(fd[0], buffer, 99);
        if (readResult != -1) {
            cout << "No error for read: " << readResult << endl;
        } else {
            cout << "Errno for read (" << readResult << "): " << errno << endl;
        }
        cout << "Buffer: " << buffer << endl;

        if (close(fd[0]) != -1) {
            cout << "No error for close fd 0." << endl;
        } else {
            cout << "Errno for close fd 0: " << errno << endl;
        }
        if (close(fd[1]) != -1) {
            cout << "No error for close fd 1." << endl;
        } else {
            cout << "Errno for close fd 1: " << errno << endl;
        }

    }
    return EXIT_SUCCESS;
}
