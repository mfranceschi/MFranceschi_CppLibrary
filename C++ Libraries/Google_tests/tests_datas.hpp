//
// Created by mfran on 02/10/2019.
//

#ifndef MYWORKS_TEST0_TESTS_DATAS_HPP
#define MYWORKS_TEST0_TESTS_DATAS_HPP

#include "gtest/gtest.h"
#include "../File.hpp"
#include "../Toolbox.hpp"

#define MIDDLESIZE_RAW      "aom_v.scx"
#define UNEXISTING_RAW      "unexisting._tut"
#define SMALL_UTF16LE_RAW   "Small_utf16le.txt"
#define TEMP_RAW            "I_AM_TEMP"
#define _Make_Fname(raw)    FNAME_PREFIX raw

#ifdef _WIN32 // WIN32
    #ifdef UNICODE
        #define FNAME_PREFIX LR"path(..\..\Google_tests\files\)path"
    #else
        #define FNAME_PREFIX R"path(..\..\Google_tests\files\)path"
    #endif
    #define I_Want_Mem_Leaks
#else // POSIX
    #define FNAME_PREFIX "../../Google_tests/files/"
#endif

#endif //MYWORKS_TEST0_TESTS_DATAS_HPP
