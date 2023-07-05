//
// Created by MartinF on 05/07/2023.
//

#include <vector>

#include "MF/FixedLengthVector.hpp"
#include "tests_data.hpp"

using namespace MF::Containers;

TEST(Containers, fixed_length_vector_full) {
    FixedLengthVector<int> fixedLengthVector(2);

    EXPECT_EQ(fixedLengthVector[0], 0);
    EXPECT_EQ(fixedLengthVector.at(0), 0);

    fixedLengthVector[0] = 4;
    EXPECT_EQ(fixedLengthVector[0], 4);
    EXPECT_EQ(fixedLengthVector.at(0), 4);

    EXPECT_EQ(fixedLengthVector.size(), 2);
    EXPECT_EQ(fixedLengthVector.capacity(), 2);
    EXPECT_EQ(fixedLengthVector.max_size(), 2);
    EXPECT_FALSE(fixedLengthVector.empty());

    EXPECT_EQ(&fixedLengthVector.front(), fixedLengthVector.data());
    EXPECT_EQ(fixedLengthVector.begin(), fixedLengthVector.data());
    EXPECT_EQ(fixedLengthVector.cbegin(), fixedLengthVector.data());

    EXPECT_EQ(&fixedLengthVector.back(), fixedLengthVector.data() + 1);
    EXPECT_EQ(fixedLengthVector.end(), fixedLengthVector.data() + 2);
    EXPECT_EQ(fixedLengthVector.cend(), fixedLengthVector.data() + 2);
}
