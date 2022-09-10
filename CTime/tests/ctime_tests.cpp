//
// Created by MartinF on 03/07/2022.
//

#include <chrono>

#include "MF/CTime.hpp"
#include "tests_data.hpp"

using namespace MF::CTime;
using SystemClock_t = std::chrono::system_clock;

TEST(CTime_Time, it_returns_the_time_at_1s_close) {
    // Note: the test seems more complicated than it should be
    // because we cannot assume that 'time_t' is in seconds, according to the standard.
    auto returned_time = SystemClock_t::from_time_t(Time());

    auto now_from_chrono = SystemClock_t::now();

    auto distance_between_both = now_from_chrono - returned_time;
    EXPECT_LE(distance_between_both, std::chrono::seconds(1)) << "More than 1s of difference.";
}

TEST(CTime_Localtime, it_returns_a_value) {
    struct std::tm theDestination {};

    bool success = Localtime(theDestination);

    EXPECT_TRUE(success);
    EXPECT_NE(theDestination.tm_year, 0);
}

TEST(CTime_Gmtime, it_returns_a_value) {
    struct std::tm theDestination {};

    bool success = Gmtime(theDestination);

    EXPECT_TRUE(success);
    EXPECT_NE(theDestination.tm_year, 0);
}

TEST(CTime_LocaltimeReversed, it_returns_a_value) {
    struct std::tm currentLocalTime {};
    bool success = Localtime(currentLocalTime);
    ASSERT_TRUE(success);

    std::time_t result = LocaltimeReversed(currentLocalTime);

    EXPECT_EQ(result, Time());
}

TEST(CTime_GmtimeReversed, it_returns_a_value) {
    struct std::tm currentGmtTime {};
    bool success = Gmtime(currentGmtTime);
    ASSERT_TRUE(success);

    std::time_t result = GmtimeReversed(currentGmtTime);

    EXPECT_EQ(result, Time());
}

TEST(CTime_Difftime, it_returns_a_value) {
    double result = Difftime(10, 5);

    EXPECT_NE(result, 0.);
}

TEST(CTime_Mktime, it_returns_a_value) {
    std::tm tm1{};
    Localtime(tm1);
    std::time_t time1 = Mktime(tm1);

    EXPECT_NE(time1, std::time_t(-1));
    EXPECT_NE(time1, 0);
    EXPECT_NE(tm1.tm_year, 0);
}

TEST(CTime_Strftime, it_returns_a_value) {
    struct std::tm currentLocalTime {};
    bool success = Localtime(currentLocalTime);
    ASSERT_TRUE(success);

    std::string result = Strftime(currentLocalTime);
    EXPECT_NE(result, "");
}

TEST(CTime_Strptime, it_returns_a_value) {
    struct std::tm currentLocalTime {};
    bool success = Localtime(currentLocalTime);
    ASSERT_TRUE(success);

    std::string strfTimeResult = Strftime(currentLocalTime);
    ASSERT_NE(strfTimeResult, "");

    struct std::tm strpTimeResultTm {};
    bool result = Strptime(strpTimeResultTm, strfTimeResult.c_str());

    EXPECT_TRUE(result);
    EXPECT_NE(strpTimeResultTm.tm_year, 0);
}
