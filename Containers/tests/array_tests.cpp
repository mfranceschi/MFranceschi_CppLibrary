//
// Created by mfran on 26/09/2020.
//

#include <array>

#include "MF/Array.hpp"
#include "tests_data.hpp"

class AutoArrayTest : public ::testing::Test {
   protected:
    MF::Array<int, 9> mfArray = {1, 3, 5, 7, 9, 11, 13, 15, 17};
    std::array<int, 9> stdArray{1, 3, 5, 7, 9, 11, 13, 15, 17};
};

TEST_F(AutoArrayTest, BasicTest) {
    EXPECT_EQ(mfArray.size(), stdArray.size());
    EXPECT_EQ(mfArray.max_size(), stdArray.max_size());
    EXPECT_EQ(mfArray.empty(), stdArray.empty());
}
