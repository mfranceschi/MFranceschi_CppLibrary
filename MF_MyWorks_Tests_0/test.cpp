#include <gtest/gtest.h>
#include "pch.h"
#include "../C++/Toolbox.hpp"
#include "../C++/Toolbox.cpp"
#include "../C++/File.hpp"
#include "../C++/File.cpp"
#include "../C++/Date.hpp"
#include "../C++/Date.cpp"

// ////////////////////////////////////////////////////////////////////////////////////////////// //
//                             ALL TESTS FOR THE File MODULE                                      //
// ////////////////////////////////////////////////////////////////////////////////////////////// //


// First settings : file names, (Win) memory leaks check.
#if 1

	// File name constants
	#ifdef _WIN32 // WIN32
		#ifdef UNICODE
			#define FNAME_PREFIX LR"path(..\TestFiles\)path"
			constexpr File::filename_t FNAME_MIDDLESIZE = FNAME_PREFIX "Vikings.scx";
			constexpr File::filename_t FNAME_UNEXISTING = FNAME_PREFIX "unexisting._tut ";
			constexpr File::filename_t FNAME_SMALL_UTF16LE = FNAME_PREFIX "Small_utf16le.txt";
		#else
			#define FNAME_PREFIX R"path(..\TestFiles\)path"
			constexpr File::filename_t FNAME_MIDDLESIZE = FNAME_PREFIX "Vikings.scx";
			constexpr File::filename_t FNAME_UNEXISTING = FNAME_PREFIX "unexisting._tut ";
			constexpr File::filename_t FNAME_SMALL_UTF16LE = FNAME_PREFIX "Small_utf16le.txt";
		#endif
	
		#define I_Want_Mem_Leaks
	#else // POSIX
		constexpr File::filename_t FNAME_MIDDLESIZE = "/mnt/d/Vikings.scx";
		constexpr File::filename_t FNAME_UNEXISTING = "/mnt/c/unexisting._tut";
	#endif

	// Turn on Memory Leaks detection (Win32 only)
	#ifdef I_Want_Mem_Leaks
		#define _CRTDBG_MAP_ALLOC
		#include <stdlib.h>
		#include <crtdbg.h>
	#endif

#endif

// Structure that holds file information.
struct file_info_data
{
	File::filesize_t size;
	File::filename_t name;
	char firstByte, lastByte;
	bool exists;
	File::encoding_t encoding;
	size_t offset;

	file_info_data(File::filesize_t fsize, File::filename_t fname, char fByte, char lByte, bool ex, File::encoding_t enc, size_t offset=0) :
		size(fsize), name(fname), firstByte(fByte), lastByte(lByte), exists(ex), encoding(enc), offset(offset)
	{}
};

static file_info_data fid_middlesize(287815, FNAME_MIDDLESIZE, 'l', '\xEF', true, File::ENC_DEFAULT);
static file_info_data fid_unexisting(0, FNAME_UNEXISTING, 0, 0, false, File::ENC_UNKNOWN);
static file_info_data fid_smallfile_utf16le(38, FNAME_SMALL_UTF16LE, '\xFF', '\x00', true, File::ENC_UTF16LE, 2);

// Motherclass for File:: test cases.
class TestFile : public ::testing::Test {
protected:
	TestFile(const file_info_data& fid) :
		fid(fid)
	{}

	void CheckSize()
	{
		File::filesize_t size = File::Size(fid.name);
		if (fid.size)
			ASSERT_NE(size, 0);
		EXPECT_EQ(size, fid.size);
	}

	void CheckExists()
	{
		bool result = File::Exists(fid.name);
		if (fid.exists)
			EXPECT_TRUE(result);
		else
			EXPECT_FALSE(result);
	}

	void CheckEncoding()
	{
		EXPECT_EQ(File::Encoding(fid.name), fid.encoding);
	}

	void CheckOpen()
	{
		std::ifstream ifs;
		File::Open(ifs, fid.name);
		if (fid.exists)
		{
			ASSERT_TRUE(ifs.good());
			EXPECT_EQ(ifs.tellg(), fid.offset);
			ifs.seekg(0, std::ios_base::beg);
			EXPECT_TRUE(ifs.good());
			EXPECT_EQ(static_cast<char>(ifs.peek()), fid.firstByte);
			ifs.seekg(-1, std::ios_base::end);
			EXPECT_EQ(static_cast<char>(ifs.peek()), fid.lastByte);
		}
		else
			ASSERT_FALSE(ifs.good());
	}

	void SetUp() override
	{
		if (fid.exists)
			ASSERT_TRUE(File::Exists(fid.name));
		else
			if (File::Exists(fid.name))
				File::Delete(fid.name, true);
	}

	void TearDown() override
	{
		if (fid.exists)
			ASSERT_TRUE(File::Exists(fid.name));
		else
			ASSERT_FALSE(File::Exists(fid.name));
	}

	file_info_data fid;
};

#define CLASS_TEST_FILE(classname, fidname) \
class classname : public TestFile {\
protected: \
	classname () : TestFile( fidname ) {} \
}

CLASS_TEST_FILE(TestUnexistingFile, fid_unexisting);
CLASS_TEST_FILE(TestMiddleWeightFile, fid_middlesize);
CLASS_TEST_FILE(TestSmallFileUTF16LE, fid_smallfile_utf16le);

// Test fixtures
#if 1
TEST_F(TestMiddleWeightFile, VerifySize) {
	CheckSize();
}

TEST_F(TestMiddleWeightFile, VerifyExists) {
	CheckExists();
}

TEST_F(TestMiddleWeightFile, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(TestMiddleWeightFile, VerifyOpen) {
	CheckOpen();
}

TEST_F(TestUnexistingFile, VerifySize) {
	CheckSize();
}

TEST_F(TestUnexistingFile, VerifyExists) {
	CheckExists();
}

TEST_F(TestUnexistingFile, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(TestUnexistingFile, VerifyOpen) {
	CheckOpen();
}

TEST_F(TestSmallFileUTF16LE, VerifySize) {
	CheckSize();
}

TEST_F(TestSmallFileUTF16LE, VerifyExists) {
	CheckExists();
}

TEST_F(TestSmallFileUTF16LE, VerifyEncoding) {
	CheckEncoding();
}

TEST_F(TestSmallFileUTF16LE, VerifyOpen) {
	CheckOpen();
}

TEST(TestDir, FullTest)
{
	EXPECT_TRUE(File::IsDir(FNAME_PREFIX));
	EXPECT_FALSE(File::IsDir(FNAME_SMALL_UTF16LE));
	EXPECT_FALSE(File::IsDir(FNAME_UNEXISTING));
}
#endif

// Main function
int main(int argc, char** argv)
{
#ifdef I_Want_Mem_Leaks
	_CrtMemState states[3];
	_CrtMemCheckpoint(&states[0]);
#endif

	testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();

#ifdef I_Want_Mem_Leaks
	_CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1]))
		_CrtMemDumpStatistics(&states[2]);
#endif
	return res;
}
