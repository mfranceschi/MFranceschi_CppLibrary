//
// Created by Utilisateur on 27/03/2022.
//

#include "Filesystem_tests_commons.hpp"

TEST(ListFilesInDirectory, UsualTest) {
    std::vector<SFilename_t> ret = ListFilesInDirectory(TEST_FILES_DIR_PREFIX FILE_SEPARATOR);
    std::vector<SFilename_t> expected = {
        MAKE_FILE_NAME "Small_utf16le.txt", MAKE_FILE_NAME "aom_v.scx",
        MAKE_FILE_NAME "EmptyFolder" FILE_SEPARATOR};

    EXPECT_LIST_CONTAINS(ret, MAKE_FILE_NAME "Small_utf16le.txt");
    EXPECT_LIST_CONTAINS(ret, MAKE_FILE_NAME "aom_v.scx");
    EXPECT_LIST_CONTAINS(ret, MAKE_FILE_NAME "EmptyFolder" FILE_SEPARATOR);

    ASSERT_EQ(expected.size(), ret.size());
    for (const SFilename_t &expectedItem : expected) {
        ASSERT_LIST_CONTAINS(ret, expectedItem);
    }
}

TEST(ListFilesInDirectory, NonExistingDirectory) {
    std::vector<SFilename_t> ret = ListFilesInDirectory(FILENAME_NOT_EXISTING.c_str());

    EXPECT_EQ(0, ret.size());
}

TEST(ListFilesInDirectory, EmptyDirectory) {
    SFilename_t name = FILENAME_TEMP;
    ASSERT_TRUE(CreateDirectory(name.c_str()));

    std::vector<SFilename_t> ret = ListFilesInDirectory(name.c_str());
    EXPECT_EQ(0, ret.size());

    ASSERT_TRUE(Delete(name.c_str()));
}
