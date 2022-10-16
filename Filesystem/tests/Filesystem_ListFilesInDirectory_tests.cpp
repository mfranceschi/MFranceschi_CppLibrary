//
// Created by Utilisateur on 27/03/2022.
//

#include "Filesystem_tests_commons.hpp"

TEST(listFilesInDirectory, UsualTest) {
    std::vector<Filename_t> expected = {
        "EmptyFolder" + FILE_SEPARATOR,
        "Small_utf16le.txt",
        "aom_v.scx",
    };

    std::vector<Filename_t> ret =
        listFilesInDirectory(MF_FILESYSTEM_TESTS_FILES_DIR + FILE_SEPARATOR);

    EXPECT_THAT(ret, ::testing::ContainerEq(expected));
}

TEST(listFilesInDirectory, NonExistingDirectory) {
    std::vector<Filename_t> ret = listFilesInDirectory(FILENAME_NOT_EXISTING);

    EXPECT_THAT(ret, ::testing::IsEmpty());
}

TEST(listFilesInDirectory, EmptyDirectory) {
    Filename_t name = FILENAME_TEMP;
    ASSERT_NO_THROW(createDirectory(name));

    std::vector<Filename_t> ret = listFilesInDirectory(name + FILE_SEPARATOR);
    EXPECT_THAT(ret, ::testing::IsEmpty());

    ASSERT_NO_THROW(deleteDirectory(name));
}
