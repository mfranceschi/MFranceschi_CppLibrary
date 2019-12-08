//
// Created by mfran on 02/10/2019.
//

#ifndef MYWORKS_TEST0_TESTS_DATAS_HPP
#define MYWORKS_TEST0_TESTS_DATAS_HPP

#include "gtest/gtest.h"
#include "../File.hpp"
#include "../Toolbox.hpp"
#include <algorithm>

#define MIDDLESIZE_RAW      "aom_v.scx"
#define UNEXISTING_RAW      "unexisting._tut"
#define SMALL_UTF16LE_RAW   "Small_utf16le.txt"
#define TEMP_RAW            "I_AM_TEMP"
#define _Make_Fname(raw)    TEST_FILES_DIR_PREFIX raw

#define TEST_FILES_DIR_PREFIX MAKE_FILE_NAME ".." FILE_SEPARATOR ".." FILE_SEPARATOR "Google_tests" FILE_SEPARATOR "files" FILE_SEPARATOR

#define ASSERT_LIST_CONTAINS(container, item) ASSERT_TRUE(std::find(container.cbegin(), container.cend(), item) != container.cend())

#endif //MYWORKS_TEST0_TESTS_DATAS_HPP
