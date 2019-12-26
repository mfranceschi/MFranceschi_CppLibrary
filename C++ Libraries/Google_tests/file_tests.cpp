//
// Created by mfran on 01/10/2019.
//

#include "tests_datas.hpp"
#include "../File.hpp"
#include "../Toolbox.hpp"

#define MIDDLE_SIZE_RAW      "aom_v.scx"
#define NOT_EXISTING_RAW      "not_existing._tut"
#define SMALL_UTF16LE_RAW   "Small_utf16le.txt"
#define TEMP_RAW            "I_AM_TEMP"
#define _Make_Fname(raw)    TEST_FILES_DIR_PREFIX raw

#define TEST_FILES_DIR_PREFIX MAKE_FILE_NAME ".." FILE_SEPARATOR ".." FILE_SEPARATOR "Google_tests" FILE_SEPARATOR "files" FILE_SEPARATOR


// First settings : file names, (Win) memory leaks check.
#if 1

constexpr File::filename_t FILENAME_MIDDLE_SIZE = _Make_Fname(MIDDLE_SIZE_RAW);
constexpr File::filename_t FILENAME_NOT_EXISTING = _Make_Fname(NOT_EXISTING_RAW);
constexpr File::filename_t FILENAME_SMALL_UTF16LE = _Make_Fname(SMALL_UTF16LE_RAW);
constexpr File::filename_t FILENAME_TEMP = _Make_Fname(TEMP_RAW);

// Turn on Memory Leaks detection (Win32 only)
#ifdef I_Want_Mem_Leaks
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#endif

// Structure that holds file information.
struct file_info_data
{
	File::file_size_t size;
	File::filename_t name;
	char firstByte, lastByte;
	bool exists;
	File::encoding_t encoding;
	size_t offset = 0;
};

// ////////////////////////////////////////////////////////////////////////////////////////////// //
//                             ALL TESTS FOR THE File MODULE                                      //
// ////////////////////////////////////////////////////////////////////////////////////////////// //

// All information for files being used.
static file_info_data fid_middle_size{287815, FILENAME_MIDDLE_SIZE, 'l', '\xEF', true, File::encoding_t::ENC_DEFAULT};
static file_info_data fid_not_existing{0, FILENAME_NOT_EXISTING, 0, 0, false, File::encoding_t::ENC_ERROR};
static file_info_data fid_smallfile_utf16le{38, FILENAME_SMALL_UTF16LE, '\xFF', '\x00', true, File::encoding_t::ENC_UTF16LE, 2};

// Mother class for test fixtures.
class TestFile : public ::testing::Test {
protected:
	file_info_data fid;

	explicit TestFile(const file_info_data& fid) :
		fid(fid)
	{}

	void CheckSize() {
		File::file_size_t size = File::Size(fid.name);
		if (fid.size)
			ASSERT_NE(size, 0);
		EXPECT_EQ(size, fid.size);
	}

	void CheckExists() {
		bool result = File::Exists(fid.name);
		if (fid.exists)
			EXPECT_TRUE(result);
		else
			EXPECT_FALSE(result);
	}

	void CheckEncoding() {
		EXPECT_EQ(File::Encoding(fid.name), fid.encoding);
	}

	void CheckOpen() {
		std::ifstream ifs;
		bool was_opened = File::Open(ifs, fid.name);
		if (fid.exists) {
			ASSERT_TRUE(was_opened);
			ASSERT_TRUE(ifs.good());
			EXPECT_EQ(ifs.tellg(), fid.offset);
			ifs.seekg(0, std::ios_base::beg);
			EXPECT_TRUE(ifs.good());
			EXPECT_EQ(static_cast<char>(ifs.peek()), fid.firstByte);
			ifs.seekg(-1, std::ios_base::end);
			EXPECT_EQ(static_cast<char>(ifs.peek()), fid.lastByte);
		}
		else {
			ASSERT_FALSE(was_opened);
			ASSERT_FALSE(ifs.good());
		}
	}

	void CheckIsEmpty() {
		if (!fid.exists) {
			ASSERT_TRUE(File::IsEmpty(fid.name, 1));
			ASSERT_TRUE(File::IsEmpty(fid.name));
		}
		else {
			// Max nbr of chars that will be readed.
			unsigned int chars_max = std::min(static_cast<File::file_size_t>(5), fid.size / 10);

			// Try to read an invalid number of chars.
			ASSERT_FALSE(File::IsEmpty(fid.name, 0));
			ASSERT_FALSE(File::IsEmpty(fid.name, -1));

			// Read a few times.
			for (unsigned int i = 1; i < chars_max; ++i) {
				EXPECT_FALSE(File::IsEmpty(fid.name, i)) << i;
			}
			if (fid.size <= 3 /* Value of default param. */)
				EXPECT_TRUE(File::IsEmpty(fid.name));
			else
				EXPECT_FALSE(File::IsEmpty(fid.name));
		}
	}

	void SetUp() override {
		if (fid.exists) {
			ASSERT_TRUE(File::Exists(fid.name)) << File::GetCWD() << std::endl << fid.name;
		}
		else {
			if (File::Exists(fid.name)) {
				File::Delete(fid.name, true);
			}
		}
	}

	void TearDown() override {
		if (fid.exists)
			ASSERT_TRUE(File::Exists(fid.name));
		else
			ASSERT_FALSE(File::Exists(fid.name));
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
TEST(IsDir, ActualFolder)
{
	ASSERT_TRUE(File::IsDir(TEST_FILES_DIR_PREFIX));
}

TEST(IsDir, IsAFile)
{
	File::filename_t fname = FILENAME_SMALL_UTF16LE;
	ASSERT_TRUE(File::Exists(fname));
	ASSERT_FALSE(File::IsDir(fname));
}

TEST(IsDir, Unexisting)
{
	ASSERT_FALSE(File::IsDir(FILENAME_NOT_EXISTING));
}

TEST(IsDir, NewFolder) {
	const File::filename_t& filename = FILENAME_TEMP;
	ASSERT_TRUE(File::CreateFolder(filename));
	EXPECT_TRUE(File::IsDir(filename));
	ASSERT_TRUE(File::Delete(filename, false));
}

TEST(Delete, Unexisting)
{
	ASSERT_FALSE(File::Delete(FILENAME_NOT_EXISTING));
}

TEST(Read, ThousandsOfRead) {
	File::filename_t file = fid_middle_size.name;
	constexpr long iterations = static_cast<long>(1e3); /* A thousand times (100ms approx). */
	const char* file_contents;
	for (long i = 0; i < iterations; ++i) {
		file_contents = File::Read(file);
		ASSERT_NE(file_contents, nullptr) << "Failed to OPEN at iteration " << i;
		ASSERT_TRUE(File::Read_Close(file_contents)) << "Failed to CLOSE at iteration " << i;
	}
}

TEST(FilesInDirectory, UsualTest) {
    std::vector<File::str_filename_t> ret = File::FilesInDirectory(TEST_FILES_DIR_PREFIX);
    std::vector<File::str_filename_t> expected = {
            MAKE_FILE_NAME "Small_utf16le.txt",
            MAKE_FILE_NAME "aom_v.scx",
            MAKE_FILE_NAME "EmptyFolder" FILE_SEPARATOR
    };

    ASSERT_EQ(expected.size(), ret.size());
    for (const File::str_filename_t& expectedItem : expected) {
        ASSERT_LIST_CONTAINS(ret, expectedItem);
    }
}
#endif

// Main function
int main(int argc, char** argv)
{
#ifdef I_Want_Mem_Leaks
	_CrtMemState states[3];
	_CrtMemCheckpoint(&states[0]);
#endif

	::testing::InitGoogleTest(&argc, argv);
	int res = RUN_ALL_TESTS();

#ifdef I_Want_Mem_Leaks
	_CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1]))
		_CrtMemDumpStatistics(&states[2]);
#endif
	return res;
}

