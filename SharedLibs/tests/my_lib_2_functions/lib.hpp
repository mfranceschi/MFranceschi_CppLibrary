//
// Created by Utilisateur on 10/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_MY_LIB_2_H
#define MFRANCESCHI_CPPLIBRARIES_MY_LIB_2_H

#include "MF/SharedLibsMacros.hpp"

#if MF_EXPORT_ME_2
#    define MF_SHARED_LIB_DIRECTIVE MF_EXPORT
#else
#    define MF_SHARED_LIB_DIRECTIVE MF_IMPORT
#endif

extern "C" MF_SHARED_LIB_DIRECTIVE int return_true();

extern "C" MF_SHARED_LIB_DIRECTIVE int return_param(int param);

extern "C" MF_SHARED_LIB_DIRECTIVE int create_file(const char* filename);

#endif // MFRANCESCHI_CPPLIBRARIES_MY_LIB_2_H
