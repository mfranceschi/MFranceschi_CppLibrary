//
// Created by Utilisateur on 10/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_MY_LIB_1_H
#define MFRANCESCHI_CPPLIBRARIES_MY_LIB_1_H

#include "MF/SharedLibsMacros.hpp"

#if MF_EXPORT_ME_1
#    define MF_SHARED_LIB_DIRECTIVE MF_EXPORT
#else
#    define MF_SHARED_LIB_DIRECTIVE MF_IMPORT
#endif

extern "C" {
MF_SHARED_LIB_DIRECTIVE int return_true();

MF_SHARED_LIB_DIRECTIVE int return_false();

MF_SHARED_LIB_DIRECTIVE int return_param(int param);

MF_SHARED_LIB_DIRECTIVE int create_file(const char* filename);
}

#endif // MFRANCESCHI_CPPLIBRARIES_MY_LIB_1_H
