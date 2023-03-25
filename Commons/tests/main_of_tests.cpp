//
// Created by mfran on 21/04/2020.
//

#include "tests_data.hpp"

#if MF_WINDOWS && MF_LOOK_FOR_MEMORY_LEAKS
#    include <crtdbg.h>
#    include <stdlib.h>

#    include <array>
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Get a checkpoint of the memory after Google Test has been initialized.
    _CrtMemState memoryState = {0};
    _CrtMemCheckpoint(&memoryState);
    int retval = RUN_ALL_TESTS();

    // Check for leaks after tests have run
    _CrtMemDumpAllObjectsSince(&memoryState);
    return retval;
}

#else
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
#endif
