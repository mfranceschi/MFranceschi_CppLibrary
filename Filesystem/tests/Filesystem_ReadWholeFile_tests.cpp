//
// Created by mfran on 01/10/2019.
//

#include "Filesystem_tests_commons.hpp"

using namespace MF::Filesystem;

TEST(ReadWholeFile, ManyReads) {
    SFilename_t file = fid_middle_size.name;
    constexpr long iterations = 1e4l;
    for (long i = 0; i < iterations; ++i) {
        auto fileData = ReadWholeFile(file.c_str());
        ASSERT_NE(fileData, nullptr) << "Failed to ReadWholeFile at iteration " << i;
    }
}

TEST(ReadWholeFile, StringRead) {
    file_info_data &file_used = fid_smallfile_utf16le;
    std::string callResult;
    EXPECT_TRUE(ReadWholeFileToString(file_used.name.c_str(), callResult))
        << "Failed to call ReadWholeFileToString";
    EXPECT_EQ(callResult.size(), file_used.size) << "Wrong file size";
}
