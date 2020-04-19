//
// Created by mfran on 08/12/2019.
//

#include <iostream>
#include <MFranceschi_CppLibrary.hpp>

using namespace std;

int main() {
    wcout << "Current working dir: " << File::GetCWD().c_str() << endl;

    Toolbox::Win_CheckForMemoryLeaks([] () {
        //TimingExperience::RunAll();
    });
    CommandCall call;
    call.executable = R"(C:\cygwin64\bin\pwd.exe)";
    CommandReturn cr;
    cr.returnCode = 44;
    Command(call, cr);
    wcout << "cr exit code is " << cr.returnCode << "!" << endl;
    Toolbox::PressAnyKeyToContinue();
    return EXIT_SUCCESS;
}
