//
// Created by mfran on 08/12/2019.
//

#include "File.hpp"
#include <iostream>
#include "TimingExperience.hpp"
#include "Toolbox.hpp"

using namespace std;

int main() {
    wcout << "Current working dir: " << File::GetCWD().c_str() << endl;

    Toolbox::Win_CheckForMemoryLeaks([] () {
        TimingExperience::RunAll();
    });
    Toolbox::PressAnyKeyToContinue();
    return EXIT_SUCCESS;
}
