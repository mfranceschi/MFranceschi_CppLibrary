//
// Created by MartinF on 18/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

TEST(SharedLib5Tests, it_throws) {
    AddToSearchPaths(MF_SAMPLE_LIB_5_FOLDER);

    std::shared_ptr<SharedLib> sharedLib;

    EXPECT_DEATH(sharedLib = OpenExplicitly(MF_SAMPLE_LIB_5_NAME), "Foo");
}
