//
// Created by mfran on 08/12/2019.
//

#include "src/File.hpp"
#include <iostream>
#include "src/TimingExperience.hpp"
#include "src/Toolbox.hpp"
#include "src/Command.hpp"

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
