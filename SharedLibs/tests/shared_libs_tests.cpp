//
// Created by Utilisateur on 10/04/2022.
//

#include "../../cmake-build-debug/_deps/googletest-src/googlemock/include/gmock/gmock-matchers.h"
#include "MF/SharedLibs.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

TEST(GetExtension, it_returns_a_value) {
    const auto *const value = GetExtension();

    ASSERT_THAT(value, ::testing::StartsWith("."));
}
