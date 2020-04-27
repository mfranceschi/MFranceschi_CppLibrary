//
// Created by mfran on 21/04/2020.
//

#include "tests_datas.hpp"

// Main function
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

#ifdef I_Want_Mem_Leaks
    _CrtMemState states[3];
    _CrtMemCheckpoint(&states[0]);
#endif

    int res = RUN_ALL_TESTS();

#ifdef I_Want_Mem_Leaks
    _CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1])) {
        _CrtMemDumpStatistics(&states[2]);
    }
#endif

    return res;
}

