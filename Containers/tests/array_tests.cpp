//
// Created by mfran on 26/09/2020.
//

#include <array>

#include "MF/Array.hpp"
#include "tests_data.hpp"

TEST(Containers, array_full) {
    const MF::Containers::Array<int, 9> mfArray = {1, 3, 5, 7, 9, 11, 13, 15, 17};
    const std::array<int, 9> stdArray{1, 3, 5, 7, 9, 11, 13, 15, 17};

    EXPECT_EQ(mfArray.size(), stdArray.size());
    EXPECT_EQ(mfArray.max_size(), stdArray.max_size());
    EXPECT_EQ(mfArray.empty(), stdArray.empty());
}
