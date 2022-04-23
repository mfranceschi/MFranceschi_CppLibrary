//
// Created by MartinF on 18/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

TEST(SharedLib5Tests, it_handles_exception_during_library_init) {
    AddToSearchPaths(MF_SAMPLE_LIB_5_FOLDER);

    std::shared_ptr<SharedLib> sharedLib;
#if MF_WINDOWS
    EXPECT_THROWS(sharedLib = OpenExplicitly(MF_SAMPLE_LIB_5_NAME), std::runtime_error)
#else
    EXPECT_DEATH(sharedLib = OpenExplicitly(MF_SAMPLE_LIB_5_NAME), "Load function throws");
#endif
    RemoveFromSearchPaths(MF_SAMPLE_LIB_5_FOLDER);
}
