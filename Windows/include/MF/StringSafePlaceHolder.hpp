//
// Created by mfran on 19/04/2020.
//

#if !defined(MFRANCESCHI_CPPLIBRARIES_STRINGSAFEPLACEHOLDER_HPP) && MF_WINDOWS
#    define MFRANCESCHI_CPPLIBRARIES_STRINGSAFEPLACEHOLDER_HPP

/**
 * In case we use MinGW, the header file <Strsafe.h> is not available.
 * Here are some replacements.
 */

#    if defined(_MSC_VER)
#        include <strsafe.h>
#    else
#        include <cstdio>
#        include <cstring>
#        define STRSAFE_E_INVALID_PARAMETER static_cast<int>(0x80070057)

#        define StringCchCopy(pszDest, cchDest, pszSrc) \
            ((std::strncpy(pszDest, pszSrc, cchDest)) ? 1 : -1)
#        if UNICODE
#            define StringCchPrintf _snwprintf
#        else
#            define StringCchPrintf std::snprintf
#        endif
#    endif

#endif // MFRANCESCHI_CPPLIBRARIES_STRINGSAFEPLACEHOLDER_HPP && MF_WINDOWS
