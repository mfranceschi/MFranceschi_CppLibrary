//
// Created by mfran on 19/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_STRINGSAFEPLACEHOLDER_HPP
#define MFRANCESCHI_CPPLIBRARIES_STRINGSAFEPLACEHOLDER_HPP

/**
 * In case we use MinGW, the header file <Strsafe.h> is not available.
 * Here are some replacements.
 */

#if defined(_MSC_VER)
#include <strsafe.h>
#else
#include <cstdio>
#include <cstring>
#define StringCchCopy(pszDest, cchDest, pszSrc) ((strncpy(pszDest, pszSrc, cchDest)) ? 1 : -1)
#define STRSAFE_E_INVALID_PARAMETER  0x80070057
#define StringCchPrintf std::snprintf
#endif


#endif //MFRANCESCHI_CPPLIBRARIES_STRINGSAFEPLACEHOLDER_HPP
