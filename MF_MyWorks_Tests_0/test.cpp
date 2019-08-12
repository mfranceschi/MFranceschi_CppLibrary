#include "pch.h"
#include "../C++/Toolbox.h"
#include "../C++/Toolbox.cpp"
#include "../C++/File.h"
#include "../C++/File.cpp"

#ifdef UNICODE
constexpr File::filename_t FNAME_MIDDLESIZE = L"D:\\Vikings.scx";
constexpr File::filename_t FNAME_UNEXISTING = L"C:\\unexisting._tut ";
#else
constexpr File::filename_t FNAME_MIDDLESIZE = "D:\\Vikings.scx";
constexpr File::filename_t FNAME_UNEXISTING = "C:\\unexisting._tut";
#endif

class TestFile : public ::testing::Test {
protected:
	TestFile(File::filename_t fname, File::filesize_t fsize, char firstByte, char lastByte) :
		filename(fname),
		filesize(fsize),
		firstByte(firstByte),
		lastByte(lastByte)
	{}

	File::filename_t filename;
	File::filesize_t filesize;
	char firstByte;
	char lastByte;
};

class TestMiddleWeightFile : public TestFile {
protected:
	TestMiddleWeightFile() : TestFile(FNAME_MIDDLESIZE, 287815, 'l', char(239)) {}
};

class TestUnexistingFile : public TestFile {
protected:
	TestUnexistingFile() : TestFile(FNAME_UNEXISTING, 0, 0, 0) {}
};

TEST_F(TestMiddleWeightFile, VerifySize) {
	File::filesize_t size = File::Size(filename);
	if (filesize)
		ASSERT_NE(size, 0);
	EXPECT_EQ(size, filesize);
}

TEST_F(TestMiddleWeightFile, VerifyExists) {
	EXPECT_TRUE(File::Exists(filename));
}

TEST_F(TestMiddleWeightFile, VerifyEncoding) {
	EXPECT_EQ(File::Encoding(filename), File::ENC_DEFAULT);
}

TEST_F(TestMiddleWeightFile, VerifyOpen) {
	std::ifstream ifs;
	File::Open(ifs, filename);
	ASSERT_TRUE(ifs.good());
	EXPECT_EQ(ifs.tellg(), 0);
	EXPECT_TRUE(ifs.good());
	EXPECT_EQ((ifs.peek()), firstByte);
	ifs.seekg(-1, std::ios_base::end);
	EXPECT_EQ(char(ifs.peek()), lastByte);
}

TEST_F(TestUnexistingFile, VerifySize) {
	File::filesize_t size = File::Size(filename);
	if (filesize)
		ASSERT_NE(size, 0);
	EXPECT_EQ(size, filesize);
}

TEST_F(TestUnexistingFile, VerifyExists) {
	EXPECT_FALSE(File::Exists(filename));
}

TEST_F(TestUnexistingFile, VerifyEncoding) {
	EXPECT_EQ(File::Encoding(filename), File::ENC_UNKNOWN);
}

TEST_F(TestUnexistingFile, VerifyOpen) {
	std::ifstream ifs;
	File::Open(ifs, filename);
	ASSERT_FALSE(ifs.good());
}

TEST(TestFileSize, DotFile) {
	const wchar_t* filename = L".";
	File::filesize_t size = File::Size(filename);
	ASSERT_EQ(size, 0);
}