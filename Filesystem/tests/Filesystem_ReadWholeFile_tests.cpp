//
// Created by mfran on 01/10/2019.
//

#include "Filesystem_tests_commons.hpp"

using namespace MF::Filesystem;

TEST(readWholeFile, ManyReads) {
    Filename_t file = fid_middle_size.name;
    const size_t position1 = 0;
    const size_t position2 = fid_middle_size.size - 1;

    constexpr long iterations = 1000L;
    for (long i = 0; i < iterations; ++i) {
        auto fileData = readWholeFile(file);
        ASSERT_NE(fileData, nullptr) << "Failed to readWholeFile at iteration " << i;
        ASSERT_NO_THROW(fileData->getContent()[position1]);
        ASSERT_NO_THROW(fileData->getContent()[position2]);
    }
}
