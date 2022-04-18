//
// Created by MartinF on 18/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

TEST(SharedLib5Tests, it_throws) {
    AddToSearchPaths(MF_SAMPLE_LIB_5_FOLDER);

    std::shared_ptr<SharedLib> sharedLib;

    // Currently it abort() on Unix and throws not found on Windows.
    // TODO: use signal handlers with <csignal> and signal(myFunction, SIGABRT) in Unix
    // implementations.
    // TODO: on Windows, ensure a specific exception is thrown and not element_not_found.
    EXPECT_DEATH(sharedLib = OpenExplicitly(MF_SAMPLE_LIB_5_NAME), "Load function throws");
}
