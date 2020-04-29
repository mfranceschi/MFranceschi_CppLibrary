//
// Created by mfran on 08/12/2019.
//

#include <iostream>
#include <MFranceschi_CppLibrary.hpp>
#include <Windows.h>

using namespace std;

int main() {
    wcout << "Current working dir: " << File::GetCWD().c_str() << endl;

    for (int i=0; i < 0; i++) {
        CommandCall call;
        call.executable = R"(C:\cygwin64\bin\pwd.exe)";
        call.outputChoice = OutputChoice::RETRIEVE;
        CommandReturn cr;
        cr.returnCode = 44;
        Command(call, cr);
    }
    Toolbox::PressAnyKeyToContinue();
    return EXIT_SUCCESS;
}
