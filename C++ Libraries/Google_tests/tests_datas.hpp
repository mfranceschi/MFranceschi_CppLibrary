//
// Created by mfran on 02/10/2019.
//

#ifndef MYWORKS_TEST0_TESTS_DATAS_HPP
#define MYWORKS_TEST0_TESTS_DATAS_HPP

#include "gtest/gtest.h"
#include <algorithm>

#define ASSERT_LIST_CONTAINS(container, item) ASSERT_TRUE(std::find(container.cbegin(), container.cend(), item) != container.cend())

#endif //MYWORKS_TEST0_TESTS_DATAS_HPP
