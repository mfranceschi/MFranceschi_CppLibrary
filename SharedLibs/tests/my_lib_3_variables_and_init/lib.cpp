//
// Created by Utilisateur on 16/04/2022.
//

#include "lib.hpp"

#include <fstream>

int thevariable = 1;
double theVariable = 1.;

MF_SHARED_LIB_LOAD_UNLOAD(
    [&]() {
        std::ofstream("lib3_load.txt") << "LOAD!";
        thevariable = 2;
        theVariable = 3;
    },
    []() {
        std::ofstream("lib3_unload.txt") << "UNLOAD!";
    })
