//
// Created by mfran on 01/10/2019.
//

#include <utility>

#include "Filesystem_tests_commons.hpp"

// Mother class for test fixtures.
class TestFile : public ::testing::Test {
   protected:
    file_info_data fid;

    explicit TestFile(file_info_data fid) : fid(std::move(fid)) {
    }

    void CheckSize() const {
        Filesize_t size = GetFileSize(fid.name.c_str());
        EXPECT_EQ(size, fid.size);
    }

    void CheckExists() const {
        bool result = IsFile(fid.name.c_str());
        if (fid.exists) {
            EXPECT_TRUE(result);
        } else {
            EXPECT_FALSE(result);
        }
    }

    void CheckEncoding() const {
        EXPECT_EQ(GetFileEncoding(fid.name.c_str()), fid.encoding);
    }

    void CheckOpen() const {
        std::ifstream ifs;
        bool was_opened = OpenFile(ifs, fid.name.c_str());
        if (fid.exists) {
            ASSERT_TRUE(was_opened);
            ASSERT_TRUE(ifs.good());
            EXPECT_EQ(ifs.tellg(), fid.offset);
            ifs.seekg(0, std::ios_base::beg);
            EXPECT_TRUE(ifs.good());
            EXPECT_EQ(static_cast<char>(ifs.peek()), fid.firstByte);
            ifs.seekg(-1, std::ios_base::end);
            EXPECT_EQ(static_cast<char>(ifs.peek()), fid.lastByte);
        } else {
            ASSERT_FALSE(was_opened);
            ASSERT_FALSE(ifs.good());
        }
    }

    void CheckIsEmpty() const {
        if (fid.exists) {
            // Max nbr of chars that will be read.
            unsigned int chars_max = std::min(static_cast<Filesize_t>(5), fid.size / 10);

            // Try to read an invalid number of chars.
            // Effect: check that the file exists.
            EXPECT_TRUE(IsFileReadable(fid.name.c_str(), 0));
            if (fid.size >= std::numeric_limits<unsigned char>::max()) {
                EXPECT_TRUE(IsFileReadable(fid.name.c_str(), -1));
            }

            // Read a few times.
            for (unsigned int i = 1; i < chars_max; ++i) {
                EXPECT_TRUE(IsFileReadable(fid.name.c_str(), i))
                    << "Fail of \"Read a few times\" with index " << i;
            }
            if (fid.size < 3 /* Value of default param. */) {
                EXPECT_FALSE(IsFileReadable(fid.name.c_str()));
            } else {
                EXPECT_TRUE(IsFileReadable(fid.name.c_str()));
            }
        } else {
            EXPECT_FALSE(IsFileReadable(fid.name.c_str(), 1));
            EXPECT_FALSE(IsFileReadable(fid.name.c_str()));

            // TODO maybe delete the following
            EXPECT_FALSE(IsFileReadable(fid.name.c_str(), -1));
        }
    }

    void SetUp() override {
        if (fid.exists) {
            ASSERT_TRUE(IsFile(fid.name.c_str())) << GetCWD() << std::endl << fid.name;
        } else {
            if (IsFile(fid.name.c_str())) {
                Delete(fid.name.c_str(), true);
            }
        }
    }

    void TearDown() override {
        if (fid.exists) {
            ASSERT_TRUE(IsFile(fid.name.c_str()));
        } else {
            ASSERT_FALSE(IsFile(fid.name.c_str()));
        }
    }
};

// Test classes for test fixtures
#if 1

class NotExistingFile : public TestFile {
   protected:
    NotExistingFile() : TestFile(fid_not_existing) {
    }
};

class MiddleWeightFile : public TestFile {
   protected:
    MiddleWeightFile() : TestFile(fid_middle_size) {
    }
};

class SmallFileUTF16LE : public TestFile {
   protected:
    SmallFileUTF16LE() : TestFile(fid_smallfile_utf16le) {
    }
};

#endif

// Test fixtures
#if 1
TEST_F(MiddleWeightFile, VerifySize) {
    CheckSize();
}

TEST_F(MiddleWeightFile, VerifyExists) {
    CheckExists();
}

TEST_F(MiddleWeightFile, VerifyEncoding) {
    CheckEncoding();
}

TEST_F(MiddleWeightFile, VerifyOpen) {
    CheckOpen();
}

TEST_F(MiddleWeightFile, VerifyIsEmpty) {
    CheckIsEmpty();
}

TEST_F(NotExistingFile, VerifySize) {
    CheckSize();
}

TEST_F(NotExistingFile, VerifyExists) {
    CheckExists();
}

TEST_F(NotExistingFile, VerifyEncoding) {
    CheckEncoding();
}

TEST_F(NotExistingFile, VerifyOpen) {
    CheckOpen();
}

TEST_F(NotExistingFile, VerifyIsEmpty) {
    CheckIsEmpty();
}

TEST_F(SmallFileUTF16LE, VerifySize) {
    CheckSize();
}

TEST_F(SmallFileUTF16LE, VerifyExists) {
    CheckExists();
}

TEST_F(SmallFileUTF16LE, VerifyEncoding) {
    CheckEncoding();
}

TEST_F(SmallFileUTF16LE, VerifyOpen) {
    CheckOpen();
}

TEST_F(SmallFileUTF16LE, VerifyIsEmpty) {
    CheckIsEmpty();
}

#endif

// Individual test cases
#if 1
TEST(IsDir, ActualFolder) {
    ASSERT_TRUE(IsDir(MAKE_FILE_NAME MF_FILESYSTEM_TESTS_FILES_DIR));
}

TEST(IsDir, IsAFile) {
    SFilename_t fname = FILENAME_SMALL_UTF16LE;
    ASSERT_TRUE(IsFile(fname.c_str()));
    ASSERT_FALSE(IsDir(fname.c_str()));
}

TEST(IsDir, Unexisting) {
    ASSERT_FALSE(IsDir(FILENAME_NOT_EXISTING.c_str()));
}

TEST(IsDir, NewFolder) {
    const SFilename_t &filename = FILENAME_TEMP;
    ASSERT_FALSE(IsDir(filename.c_str()));

    ASSERT_TRUE(CreateDirectory(filename.c_str()));

    EXPECT_TRUE(IsDir(filename.c_str()));
    ASSERT_TRUE(Delete(filename.c_str(), false));
}

TEST(Delete, Unexisting) {
    ASSERT_FALSE(Delete(FILENAME_NOT_EXISTING.c_str()));
}

#endif

TEST(GetCwd, itWorks) {
    SFilename_t cwd = GetCWD();
    ASSERT_FALSE(cwd.empty());
}
