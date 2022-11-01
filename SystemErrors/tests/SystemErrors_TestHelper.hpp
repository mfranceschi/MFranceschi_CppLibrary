//
// Created by Utilisateur on 02/11/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_TESTHELPER_HPP
#define MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_TESTHELPER_HPP

#include "MF/SystemErrors.hpp"
#include "tests_data.hpp"

using namespace MF::SystemErrors;

inline void checkSystemError(
    const SystemError& systemError, Paradigm expectedParadigm, long expectedError) {
    EXPECT_EQ(systemError.getParadigm(), expectedParadigm);
    EXPECT_EQ(systemError.getErrorCode(), expectedError);

    const char* theWhat = systemError.what();
    EXPECT_NE(theWhat, nullptr);
    EXPECT_STRNE(theWhat, "");
}

#endif // MFRANCESCHI_CPPLIBRARIES_SYSTEMERRORS_TESTHELPER_HPP
