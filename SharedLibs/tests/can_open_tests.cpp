//
// Created by MartinF on 29/05/2022.
//

#include "shared_libs_tests_common.hpp"

TEST(CanOpenFunction, it_returns_true_if_library_can_be_opened) {
    for (std::size_t i = 1; i < libs.size(); i++) {
        const MfSampleLib& lib = libs[i];
        if (!lib.shouldLoad) {
            continue;
        }

        AddToSearchPaths(lib.folder);

        EXPECT_TRUE(CanOpen(lib.name));

        RemoveFromSearchPaths(lib.folder);
    }
}

TEST(CanOpenFunction, it_returns_false_if_library_cannot_be_opened) {
    for (std::size_t i = 1; i < libs.size(); i++) {
        const MfSampleLib& lib = libs[i];
        if (lib.shouldLoad) {
            continue;
        }

        AddToSearchPaths(lib.folder);

        EXPECT_FALSE(CanOpen(lib.name));

        RemoveFromSearchPaths(lib.folder);
    }
}