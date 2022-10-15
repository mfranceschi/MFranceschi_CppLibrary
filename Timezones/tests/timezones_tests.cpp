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

TEST(TimeZoneName, it_returns_a_value) {
    std::string tzName;
    EXPECT_NO_THROW(tzName = getTimezoneName());
    EXPECT_GT(tzName.length(), std::size_t(1));
}

TEST(SystemTz, it_sets_and_gets_a_value) {
    std::string tzSetValue="abcde";

}