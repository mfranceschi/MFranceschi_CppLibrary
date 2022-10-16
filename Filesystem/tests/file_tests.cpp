//
// Created by mfran on 01/10/2019.
//

#include <utility>

#include "Filesystem_tests_commons.hpp"
#include "MF/SystemErrors.hpp"

// Mother class for test fixtures.
class TestFile : public ::testing::Test {
   protected:
    file_info_data fid;

    explicit TestFile(file_info_data fid) : fid(std::move(fid)) {
    }

    void CheckSize() const {
        if (fid.exists) {
            Filesize_t size = getFileSize(fid.name);
            EXPECT_EQ(size, fid.size);
        } else {
            EXPECT_THROW(getFileSize(fid.name), MF::SystemErrors::SystemError);
        }
    }

    void CheckExists() const {
        bool result = isFile(fid.name);
        if (fid.exists) {
            EXPECT_TRUE(result);
        } else {
            EXPECT_FALSE(result);
        }
    }

    void CheckEncoding() const {
        if (fid.exists) {
            EXPECT_EQ(getFileEncoding(fid.name), fid.encoding);
        } else {
            EXPECT_THROW(getFileEncoding(fid.name), MF::SystemErrors::SystemError);
        }
    }

    void CheckOpen() const {
        if (fid.exists) {
            auto openedFile = openFile(fid.name);
            auto &ifs = *openedFile;
            ASSERT_TRUE(ifs.good());
            EXPECT_EQ(ifs.tellg(), fid.offset);
            ifs.seekg(0, std::ios_base::beg);
            EXPECT_TRUE(ifs.good());
            EXPECT_EQ(static_cast<char>(ifs.peek()), fid.firstByte);
            ifs.seekg(-1, std::ios_base::end);
            EXPECT_EQ(static_cast<char>(ifs.peek()), fid.lastByte);
        } else {
            EXPECT_THROW(openFile(fid.name), MF::SystemErrors::SystemError);
        }
    }

    void SetUp() override {
        if (fid.exists) {
            ASSERT_TRUE(isFile(fid.name)) << getCWD() << std::endl << fid.name;
        } else {
            if (isFile(fid.name)) {
                deleteFile(fid.name);
            }
        }
    }

    void TearDown() override {
        if (fid.exists) {
            ASSERT_TRUE(isFile(fid.name));
        } else {
            ASSERT_FALSE(isFile(fid.name));
        }
    }
};

// Test classes for test fixtures
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

// Test fixtures
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

// Individual test cases
TEST(isDir, ActualFolder) {
    ASSERT_TRUE(isDir(MF_FILESYSTEM_TESTS_FILES_DIR));
}

TEST(isDir, IsAFile) {
    Filename_t fname = FILENAME_SMALL_UTF16LE;
    ASSERT_TRUE(isFile(fname));
    ASSERT_FALSE(isDir(fname));
}

TEST(isDir, Unexisting) {
    ASSERT_FALSE(isDir(FILENAME_NOT_EXISTING));
}

TEST(isDir, NewFolder) {
    const Filename_t &filename = FILENAME_TEMP;
    ASSERT_FALSE(isDir(filename));

    ASSERT_NO_THROW(createDirectory(filename));

    EXPECT_TRUE(isDir(filename));
    ASSERT_NO_THROW(deleteDirectory(filename));
}

TEST(deleteFile, Unexisting) {
    ASSERT_THROW(deleteFile(FILENAME_NOT_EXISTING), MF::SystemErrors::SystemError);
}

TEST(GetCwd, itWorks) {
    Filename_t cwd = getCWD();
    ASSERT_FALSE(cwd.empty());
}
