//
// Created by Utilisateur on 16/04/2022.
//

#include "my_lib_3_variables_and_init.hpp"

#include <fstream>

int the_variable = 1;
double theVariable = 1.;

MF_SHARED_LIB_LOAD_UNLOAD(
    [&]() {
        std::ofstream("temp_load.txt") << "LOAD!";
        the_variable = 2;
        theVariable = 3.;
    },
    []() {
        std::ofstream("temp_unload.txt") << "UNLOAD!";
    })
