//
// Created by mfran on 01/10/2019.
//

#include "Filesystem_tests_commons.hpp"

using namespace MF::Filesystem;

TEST(readWholeFile, ManyReads) {
    Filename_t file = fid_middle_size.name;
    constexpr long iterations = 1000L;
    for (long i = 0; i < iterations; ++i) {
        auto fileData = readWholeFile(file);
        ASSERT_NE(fileData, nullptr) << "Failed to readWholeFile at iteration " << i;
    }
}
