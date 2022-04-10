//
// Created by Utilisateur on 10/04/2022.
//

#include "my_lib_1.hpp"

#include <fstream>

MF_SHARED_LIB_DIRECTIVE int return_true() {
    return 1;
}

MF_SHARED_LIB_DIRECTIVE int return_false() {
    return 0;
}

MF_SHARED_LIB_DIRECTIVE int return_param(int param) {
    return param;
}

MF_SHARED_LIB_DIRECTIVE int create_file(const char* filename) {
    std::ofstream ofs(filename);
    return static_cast<int>(!ofs.fail());
}
