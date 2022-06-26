//
// Created by MartinF on 18/06/2022.
//

#include <functional>

#include "MF/Timezones.hpp"
#include "tests_data.hpp"

using namespace MF::Timezones;

TEST(TimezoneOffset, it_returns_a_value) {
    tzSet();
    std::function<decltype(getTimezoneOffset)>::result_type tzOffset;
    EXPECT_NO_THROW(tzOffset = getTimezoneOffset());
}

TEST(DstOffset, it_returns_a_value) {
    tzSet();
    std::function<decltype(getDstOffset)>::result_type dstOffset;
    EXPECT_NO_THROW(dstOffset = getDstOffset());
}
