#include <gtest/gtest.h>
#include "pch.h"
#include "../C++/Toolbox.hpp"
#include "../C++/Toolbox.cpp"
#include "../C++/File.hpp"
#include "../C++/File.cpp"

#ifdef _WIN32 // WIN32
	#ifdef UNICODE
		constexpr File::filename_t FNAME_MIDDLESIZE = L"D:\\Vikings.scx";
		constexpr File::filename_t FNAME_UNEXISTING = L"C:\\unexisting._tut ";
	#else
		constexpr File::filename_t FNAME_MIDDLESIZE = "D:\\Vikings.scx";
		constexpr File::filename_t FNAME_UNEXISTING = "C:\\unexisting._tut";
	#endif
	
	#define I_Want_Mem_Leaks
#else // POSIX
	constexpr File::filename_t FNAME_MIDDLESIZE = "/mnt/d/Vikings.scx";
	constexpr File::filename_t FNAME_UNEXISTING = "/mnt/c/unexisting._tut";
#endif

#ifdef I_Want_Mem_Leaks
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

struct file_info_data
{
	File::filesize_t size;
	File::filename_t name;
	char firstByte, lastByte;
	bool exists;
	File::encoding_t encoding;

	file_info_data(File::filesize_t fsize, File::filename_t fname, char fByte, char lByte, bool ex, File::encoding_t enc) :
		size(fsize), name(fname), firstByte(fByte), lastByte(lByte), exists(ex), encoding(enc)
	{}
};

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
			EXPECT_EQ(ifs.tellg(), 0);
			EXPECT_TRUE(ifs.good());
			EXPECT_EQ((ifs.peek()), fid.firstByte);
			ifs.seekg(-1, std::ios_base::end);
			EXPECT_EQ(char(ifs.peek()), fid.lastByte);
		}
		else
			ASSERT_FALSE(ifs.good());
	}

	file_info_data fid;
};

class TestMiddleWeightFile : public TestFile {
protected:
	TestMiddleWeightFile() : 
		TestFile(file_info_data(287815, FNAME_MIDDLESIZE, 'l', char(239), true, File::ENC_DEFAULT))
	{}
};

class TestUnexistingFile : public TestFile {
protected:
	TestUnexistingFile() : 
		TestFile(file_info_data(0, FNAME_UNEXISTING, 0, 0, false, File::ENC_UNKNOWN))
	{}

	void SetUp() override 
	{
		if (File::Exists(fid.name))
			File::Delete(fid.name);
	}

	void TearDown() override
	{
		ASSERT_FALSE(File::Exists(fid.name));
	}
};

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
