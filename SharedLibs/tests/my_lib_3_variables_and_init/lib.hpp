//
// Created by Utilisateur on 16/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_LIB_HPP
#define MFRANCESCHI_CPPLIBRARIES_LIB_HPP

#include "MF/SharedLibsMacros.hpp"

#if MF_EXPORT_ME_3
#    define MF_SHARED_LIB_DIRECTIVE MF_EXPORT
#else
#    define MF_SHARED_LIB_DIRECTIVE MF_IMPORT
#endif

extern "C" {
MF_SHARED_LIB_DIRECTIVE extern int thevariable;
MF_SHARED_LIB_DIRECTIVE extern double theVariable;
}

#endif // MFRANCESCHI_CPPLIBRARIES_LIB_HPP
