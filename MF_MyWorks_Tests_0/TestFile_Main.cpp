#include <gtest/gtest.h>
#include "pch.h"
#include "../C++/Toolbox.hpp"
#include "../C++/Toolbox.cpp"
#include "../C++/File.hpp"
#include "../C++/File.cpp"

#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // min

// ////////////////////////////////////////////////////////////////////////////////////////////// //
//                             ALL TESTS FOR THE File MODULE                                      //
// ////////////////////////////////////////////////////////////////////////////////////////////// //

// All informations for files being used.
static file_info_data fid_middlesize(287815, FNAME_MIDDLESIZE, 'l', '\xEF', true, File::ENC_DEFAULT);
static file_info_data fid_unexisting(0, FNAME_UNEXISTING, 0, 0, false, File::ENC_ERROR);
static file_info_data fid_smallfile_utf16le(38, FNAME_SMALL_UTF16LE, '\xFF', '\x00', true, File::ENC_UTF16LE, 2);

static const char* smallfilecontent = "";

// Motherclass for test fixtures.
class TestFile : public ::testing::Test {
protected:
	file_info_data fid;

	explicit TestFile(const file_info_data& fid) :
		fid(fid)
	{}

	void CheckSize() {
		File::filesize_t size = File::Size(fid.name);
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
			unsigned int chars_max = std::min(static_cast<File::filesize_t>(5), fid.size / 10);

			// Try to read an invalid number of chars.
			try {
				auto tmp = File::IsEmpty(fid.name, 0);
				FAIL() << tmp;
			} catch (void*) {}
			try {
				auto tmp = File::IsEmpty(fid.name, -1);
				FAIL() << tmp;
			}
			catch (void*) {}

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
			ASSERT_TRUE(File::Exists(fid.name));
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
class UnexistingFile : public TestFile {
protected:
	UnexistingFile() : TestFile(fid_unexisting) {}
};

class MiddleWeightFile : public TestFile {
protected:
	MiddleWeightFile() : TestFile(fid_middlesize) {}
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

TEST_F(UnexistingFile, VerifySize) {
	CheckSize();
}

TEST_F(UnexistingFile, VerifyExists) {
	CheckExists();
}

TEST_F(UnexistingFile, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(UnexistingFile, VerifyOpen) {
	CheckOpen();
}

TEST_F(UnexistingFile, VerifyIsEmpty) {
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
	ASSERT_TRUE(File::IsDir(FNAME_PREFIX));
}

TEST(IsDir, IsAFile)
{
	ASSERT_FALSE(File::IsDir(FNAME_SMALL_UTF16LE));
}

TEST(IsDir, Unexisting)
{
	ASSERT_FALSE(File::IsDir(FNAME_UNEXISTING));
}

TEST(IsDir, NewFolder) {
	const File::filename_t& filename = FNAME_TEMP;
	ASSERT_TRUE(File::CreateFolder(filename));
	EXPECT_TRUE(File::IsDir(filename));
	ASSERT_TRUE(File::Delete(filename, false));
}

TEST(Delete, Unexisting)
{
	ASSERT_FALSE(File::Delete(FNAME_UNEXISTING));
}

TEST(Read, ThousandsOfRead) {
	File::filename_t file = fid_middlesize.name;
	long iterations = static_cast<long>(1e3); /* A thousand times (100ms approx). */
	const char* file_contents;
	for (long i = 0; i < iterations; ++i) {
		file_contents = File::Read(file);
		ASSERT_NE(file_contents, nullptr) << "Failed to OPEN at iteration " << i;
		ASSERT_TRUE(File::Read_Close(file_contents)) << "Failed to CLOSE at iteration " << i;
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
	auto res = RUN_ALL_TESTS();

#ifdef I_Want_Mem_Leaks
	_CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1]))
		_CrtMemDumpStatistics(&states[2]);
#endif
	return res;
}
