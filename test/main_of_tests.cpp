//
// Created by mfran on 21/04/2020.
//

#include <gtest/gtest.h>

// Turn on Memory Leaks detection (Win32 only)
#ifdef I_Want_Mem_Leaks
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Main function
int main(int argc, char** argv)
{
#ifdef I_Want_Mem_Leaks
    _CrtMemState states[3];
	_CrtMemCheckpoint(&states[0]);
#endif

    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();

#ifdef I_Want_Mem_Leaks
    _CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1]))
		_CrtMemDumpStatistics(&states[2]);
#endif
    return res;
}

