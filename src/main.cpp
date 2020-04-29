//
// Created by mfran on 08/12/2019.
//

#include <iostream>
#include <MFranceschi_CppLibrary.hpp>
#include <Windows.h>

using namespace std;

int main() {
    wcout << "Current working dir: " << File::GetCWD().c_str() << endl;

    Toolbox::Win_CheckForMemoryLeaks([] () {
        CommandCall call;
        call.executable = R"(C:\cygwin64\bin\pwd.exe)";
        CommandReturn cr;
        cr.returnCode = 44;
        Command(call, cr);
        wcout << "exit code is " << cr.returnCode << "!" << endl;
        cout << std::to_string(GetLastError()) << endl;
    });

    return EXIT_SUCCESS;
}
