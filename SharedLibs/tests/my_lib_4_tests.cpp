//
// Created by MartinF on 18/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

TEST(SharedLib4Tests, it_opens_and_closes_fine) {
    std::shared_ptr<SharedLib> sharedLib;

    AddToSearchPaths(MF_SAMPLE_LIB_4_FOLDER);

    EXPECT_NO_THROW(sharedLib = OpenExplicitly(MF_SAMPLE_LIB_4_NAME));
    EXPECT_NO_THROW(sharedLib.reset());

    RemoveFromSearchPaths(MF_SAMPLE_LIB_4_FOLDER);
}
