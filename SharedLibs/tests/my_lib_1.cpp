//
// Created by Utilisateur on 10/04/2022.
//

#include "my_lib_1.hpp"

#include <fstream>

int return_true() {
    return 1;
}

int return_param(int param) {
    return param;
}

int create_file(const char* filename) {
    std::ofstream ofs(filename);
    return static_cast<int>(!ofs.fail());
}
