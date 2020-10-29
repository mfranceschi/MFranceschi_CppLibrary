//
// Created by mfran on 26/09/2020.
//

#include <array>

#include "tests_datas.hpp"

class AutoArrayTest : public ::testing::Test {
protected:
    Array<int, 9> mfArray;
    std::array<int, 9> stdArray;

    void SetUp() override;
};

void AutoArrayTest::SetUp() {
#define MySetupData {1, 3, 5, 7, 9, 11, 13, 15, 17}
    mfArray = MySetupData;
    stdArray = MySetupData;
#undef MySetupData
}

TEST_F(AutoArrayTest, BasicTest) {
    Array<int, 9> mfArray;
    std::array<int, 9> stdArray;

#define MySetupData {1, 3, 5, 7, 9, 11, 13, 15, 17}
    mfArray = MySetupData;
    stdArray = MySetupData;
#undef MySetupData

    EXPECT_EQ(mfArray.size(), stdArray.size());
    EXPECT_EQ(mfArray.max_size(), stdArray.max_size());
    EXPECT_EQ(mfArray.empty(), stdArray.empty());
}