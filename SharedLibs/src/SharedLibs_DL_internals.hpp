//
// Created by Utilisateur on 13/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SHAREDLIBS_DL_INTERNALS_HPP
#define MFRANCESCHI_CPPLIBRARIES_SHAREDLIBS_DL_INTERNALS_HPP

#include <set>
#include <string>

namespace MF
{
    namespace SharedLibs
    {
        namespace internals
        {
            const std::set<std::string>& getSearchPaths();
        }
    } // namespace SharedLibs
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_SHAREDLIBS_DL_INTERNALS_HPP
