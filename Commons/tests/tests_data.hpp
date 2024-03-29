//
// Created by mfran on 02/10/2019.
//

#ifndef MYWORKS_TEST0_TESTS_DATAS_HPP
#define MYWORKS_TEST0_TESTS_DATAS_HPP

#include <algorithm>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Turn on Memory Leaks detection (Win32 and MSVC only)
#if defined(_MSC_VER) && defined(I_Want_Mem_Leaks)
#    define _CRTDBG_MAP_ALLOC
#    include <crtdbg.h>

#    include <cstdlib>
#endif

#define ASSERT_LIST_CONTAINS(container, item)                                              \
    ASSERT_TRUE(std::find(container.cbegin(), container.cend(), item) != container.cend()) \
        << "Container does not contain '" << #item << "'"
#define EXPECT_LIST_CONTAINS(container, item)                                              \
    EXPECT_TRUE(std::find(container.cbegin(), container.cend(), item) != container.cend()) \
        << "Container does not contain '" << #item << "'"

#endif // MYWORKS_TEST0_TESTS_DATAS_HPP
