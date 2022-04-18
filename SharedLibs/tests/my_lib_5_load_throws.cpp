//
// Created by MartinF on 18/04/2022.
//

#include <exception>

#include "MF/SharedLibsMacros.hpp"

MF_SHARED_LIB_LOAD_UNLOAD(
    []() {
        throw std::runtime_error("Load function throws.");
    },
    nullptr)
