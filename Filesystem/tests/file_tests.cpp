//
// Created by mfran on 01/10/2019.
//

#include <utility>

#include "MF/Filesystem.hpp"
#include "tests_data.hpp"

#if defined(UNICODE)
#define MIDDLE_SIZE_RAW      L"aom_v.scx"
#define NOT_EXISTING_RAW      L"not_existing._tut"
#define SMALL_UTF16LE_RAW   L"Small_utf16le.txt"
#define TEMP_RAW            L"I_AM_TEMP"
#else
#define MIDDLE_SIZE_RAW      "aom_v.scx"
#define NOT_EXISTING_RAW      "not_existing._tut"
#define SMALL_UTF16LE_RAW   "Small_utf16le.txt"
#define TEMP_RAW            "I_AM_TEMP"
#endif

#define TEST_FILES_DIR_PREFIX FILES_DIR // MAKE_FILE_NAME ".." FILE_SEPARATOR ".." FILE_SEPARATOR "test" FILE_SEPARATOR "files" FILE_SEPARATOR

// First settings : file names, (Win) memory leaks check.
#if 1

namespace File = MF::Filesystem;

const File::SFilename_t FILENAME_MIDDLE_SIZE = File::MakeFilename(false, false, 2, TEST_FILES_DIR_PREFIX,
                                                                  MIDDLE_SIZE_RAW);
const File::SFilename_t FILENAME_NOT_EXISTING = File::MakeFilename(false, false, 2, TEST_FILES_DIR_PREFIX,
                                                                   NOT_EXISTING_RAW);
const File::SFilename_t FILENAME_SMALL_UTF16LE = File::MakeFilename(false, false, 2, TEST_FILES_DIR_PREFIX,
                                                                    SMALL_UTF16LE_RAW);
const File::SFilename_t FILENAME_TEMP = File::MakeFilename(false, false, 2, TEST_FILES_DIR_PREFIX, TEMP_RAW);

#endif

// Structure that holds file information.
struct file_info_data {
    File::Filesize_t size;
    File::SFilename_t name;
    char firstByte, lastByte;
    bool exists;
    File::Encoding_t encoding;
    size_t offset = 0;
};

// ////////////////////////////////////////////////////////////////////////////////////////////// //
//                             ALL TESTS FOR THE File MODULE                                      //
// ////////////////////////////////////////////////////////////////////////////////////////////// //

// All information for files being used.
static file_info_data fid_middle_size{287815, FILENAME_MIDDLE_SIZE, 'l', '\xEF', true, File::Encoding_e::ENC_DEFAULT};
static file_info_data fid_not_existing{0, FILENAME_NOT_EXISTING, 0, 0, false, File::Encoding_e::ENC_ERROR};
static file_info_data fid_smallfile_utf16le{38, FILENAME_SMALL_UTF16LE, '\xFF', '\x00', true,
                                            File::Encoding_e::ENC_UTF16LE, 2};

// Mother class for test fixtures.
class TestFile : public ::testing::Test {
protected:
    file_info_data fid;

    explicit TestFile(file_info_data fid) :
            fid(std::move(fid)) {}

    void CheckSize() const {
        File::Filesize_t size = File::Size(fid.name.c_str());
        if (fid.size)
            ASSERT_NE(size, 0);
        EXPECT_EQ(size, fid.size);
    }

    void CheckExists() const {
        bool result = File::Exists(fid.name.c_str());
        if (fid.exists)
            EXPECT_TRUE(result);
        else
            EXPECT_FALSE(result);
    }

    void CheckEncoding() const {
        EXPECT_EQ(File::Encoding(fid.name.c_str()), fid.encoding);
    }

    void CheckOpen() const {
        std::ifstream ifs;
        bool was_opened = File::Open(ifs, fid.name.c_str());
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
        if (!fid.exists) {
            EXPECT_FALSE(File::CanReadFile(fid.name.c_str(), 1));
            EXPECT_FALSE(File::CanReadFile(fid.name.c_str()));
            EXPECT_FALSE(File::CanReadFile(fid.name.c_str(), -1));
        } else {
            // Max nbr of chars that will be readed.
            unsigned int chars_max = std::min(static_cast<File::Filesize_t>(5), fid.size / 10);

            // Try to read an invalid number of chars.
            // Effect: check that the file exists.
            EXPECT_TRUE(File::CanReadFile(fid.name.c_str(), 0));
            EXPECT_TRUE(File::CanReadFile(fid.name.c_str(), -1));

            // Read a few times.
            for (unsigned int i = 1; i < chars_max; ++i) {
                EXPECT_TRUE(File::CanReadFile(fid.name.c_str(), i)) << "Fail of \"Read a few times\" with index " << i;
            }
            if (fid.size < 3 /* Value of default param. */)
                EXPECT_FALSE(File::CanReadFile(fid.name.c_str()));
            else
                EXPECT_TRUE(File::CanReadFile(fid.name.c_str()));
        }
    }

    void SetUp() override {
        if (fid.exists) {
            ASSERT_TRUE(File::Exists(fid.name.c_str())) << File::GetCWD() << std::endl << fid.name;
        } else {
            if (File::Exists(fid.name.c_str())) {
                File::Delete(fid.name.c_str(), true);
            }
        }
    }

    void TearDown() override {
        if (fid.exists)
            ASSERT_TRUE(File::Exists(fid.name.c_str()));
        else
            ASSERT_FALSE(File::Exists(fid.name.c_str()));
    }

};

// Test classes for test fixtures
#if 1

class NotExistingFile : public TestFile {
protected:
    NotExistingFile() : TestFile(fid_not_existing) {}
};

class MiddleWeightFile : public TestFile {
protected:
    MiddleWeightFile() : TestFile(fid_middle_size) {}
};

class SmallFileUTF16LE : public TestFile {
protected:
    SmallFileUTF16LE() : TestFile(fid_smallfile_utf16le) {}
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
    ASSERT_TRUE(File::IsDir(TEST_FILES_DIR_PREFIX));
}

TEST(IsDir, IsAFile) {
    File::SFilename_t fname = FILENAME_SMALL_UTF16LE;
    ASSERT_TRUE(File::Exists(fname.c_str()));
    ASSERT_FALSE(File::IsDir(fname.c_str()));
}

TEST(IsDir, Unexisting) {
    ASSERT_FALSE(File::IsDir(FILENAME_NOT_EXISTING.c_str()));
}

TEST(IsDir, NewFolder) {
    const File::SFilename_t &filename = FILENAME_TEMP;
    ASSERT_TRUE(File::CreateFolder(filename.c_str()));
    EXPECT_TRUE(File::IsDir(filename.c_str()));
    ASSERT_TRUE(File::Delete(filename.c_str(), false));
}

TEST(Delete, Unexisting) {
    ASSERT_FALSE(File::Delete(FILENAME_NOT_EXISTING.c_str()));
}

TEST(Read, ManyReads) {
    File::SFilename_t file = fid_middle_size.name;
    constexpr long iterations = 1e1l; // A thousand times (100ms approx).
    for (long i = 0; i < iterations; ++i) {
        auto filedata = File::Read(file.c_str());
        ASSERT_NE(filedata, nullptr) << "Failed to OPEN at iteration " << i;
        File::Read_Close(filedata);
    }
}

TEST(Read, StringRead) {
    file_info_data &file_used = fid_smallfile_utf16le;
    std::string callResult;
    EXPECT_TRUE(File::ReadToString(file_used.name.c_str(), callResult)) << "Failed to call ReadToString";
    EXPECT_EQ(callResult.size(), file_used.size) << "Wrong file size";
}

TEST(FilesInDirectory, UsualTest) {
    std::vector<File::SFilename_t> ret = File::FilesInDirectory(TEST_FILES_DIR_PREFIX FILE_SEPARATOR);
    std::vector<File::SFilename_t> expected = {
            MAKE_FILE_NAME "Small_utf16le.txt",
            MAKE_FILE_NAME "aom_v.scx",
            MAKE_FILE_NAME "EmptyFolder" FILE_SEPARATOR
    };

    EXPECT_LIST_CONTAINS(ret, MAKE_FILE_NAME "Small_utf16le.txt");
    EXPECT_LIST_CONTAINS(ret, MAKE_FILE_NAME "aom_v.scx");
    EXPECT_LIST_CONTAINS(ret, MAKE_FILE_NAME "EmptyFolder" FILE_SEPARATOR);

    ASSERT_EQ(expected.size(), ret.size());
    for (const File::SFilename_t &expectedItem: expected) {
        ASSERT_LIST_CONTAINS(ret, expectedItem);
    }
}

#endif

TEST(GetCwd, itWorks) {
    File::SFilename_t cwd = File::GetCWD();
    ASSERT_FALSE(cwd.empty());
}
