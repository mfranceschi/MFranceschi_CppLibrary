//
// Created by mfran on 01/10/2019.
//

#include <chrono>
#include <cmath>

#include "MF/Datetime.hpp"
#include "MF/Timezones.hpp"
#include "tests_data.hpp"

using namespace MF::Datetime;

class GetterSetterTests : public ::testing::Test {
   protected:
    Date d1;
    Date d1_original;
    const int year = 2019 - 1900;
    const int month = SEPTEMBER;
    const int monthday = 16;
    const int hour = 17;
    const int minutes = 41;
    const int seconds = 37;
    const int dst_flag = DST_ON;

    inline void SetUp() override;
};

void GetterSetterTests::SetUp() {
    static constexpr const char *tzForTests =
#if MF_WINDOWS
        "CET-1CEST";
#else
        "Europe/Paris";
#endif
    MF::Timezones::setSystemTz(tzForTests);
    MF::Timezones::initSystemTz();

    d1 = Date(year, month, monthday, hour, minutes, seconds, dst_flag);
    d1_original = d1;
}

namespace StaticMethodsTesting
{
    using clock = std::chrono::system_clock;

    TEST(IsLeapYear, EffectiveLeapYears) {
        EXPECT_TRUE(IsLeapYear(0));
        EXPECT_TRUE(IsLeapYear(2000));
        EXPECT_TRUE(IsLeapYear(2004));
    }

    TEST(IsLeapYear, NotLeapYears) {
        EXPECT_FALSE(IsLeapYear(1));
        EXPECT_FALSE(IsLeapYear(2));
        EXPECT_FALSE(IsLeapYear(3));
        EXPECT_FALSE(IsLeapYear(100));
        EXPECT_FALSE(IsLeapYear(2100));
    }

    TEST(DaysInMonth, OtherMonths) {
        // Months of 31 days.
        EXPECT_EQ(DaysInMonth(JANUARY), 31);
        EXPECT_EQ(DaysInMonth(MARCH), 31);
        EXPECT_EQ(DaysInMonth(MAY), 31);
        EXPECT_EQ(DaysInMonth(MAY), 31);
        EXPECT_EQ(DaysInMonth(JULY), 31);
        EXPECT_EQ(DaysInMonth(AUGUST), 31);
        EXPECT_EQ(DaysInMonth(OCTOBER), 31);
        EXPECT_EQ(DaysInMonth(DECEMBER), 31);

        // Months of 30 days.
        EXPECT_EQ(DaysInMonth(APRIL), 30);
        EXPECT_EQ(DaysInMonth(JUNE), 30);
        EXPECT_EQ(DaysInMonth(SEPTEMBER), 30);
        EXPECT_EQ(DaysInMonth(NOVEMBER), 30);
    }

    TEST(DaysInMonth, WrongInput) {
        EXPECT_EQ(DaysInMonth(-1), 0);
        EXPECT_EQ(DaysInMonth(-2), 0);
        EXPECT_EQ(DaysInMonth(12), 0);
        EXPECT_EQ(DaysInMonth(123), 0);
    }

    TEST(DaysInMonth, February) {
        EXPECT_EQ(DaysInMonth(FEBRUARY, 0), 29); // Leap year
        EXPECT_EQ(DaysInMonth(FEBRUARY, 1), 28); // Non-leap year
    }

    TEST(Now_Timestamp, FullTest) {
        time_t now_to_check = Now_Timestamp();
        time_t now_from_chrono = clock::to_time_t(clock::now());
        time_t distance = now_from_chrono - now_to_check;
        if (distance < 0) {
            distance *= -1;
        }
        EXPECT_TRUE(distance <= 1);
    }

    TEST(Now, FullTest) {
        std::time_t now_to_check = time_t(Date::Now());
        time_t now_from_chrono = clock::to_time_t(clock::now());

        // The test = ensure the distance between both is no more than 1s.
        // We are in C++ so more than 1s between these 2 instructions is undoubtedly a failure.
        time_t distance = std::abs(now_from_chrono - now_to_check);
        EXPECT_LE(distance, 1) << "Date::now() to time_t gives " << now_to_check
                               << ", while clock::now() to time_t gives " << now_from_chrono;
    }
} // namespace StaticMethodsTesting

TEST_F(GetterSetterTests, TestGetters) {
    EXPECT_EQ(d1.year(), year);
    EXPECT_EQ(d1.month(), month);
    EXPECT_EQ(d1.day_month(), monthday);
    EXPECT_EQ(d1.hours(), hour);
    EXPECT_EQ(d1.minutes(), minutes);
    EXPECT_EQ(d1.seconds(), seconds);
    EXPECT_EQ(d1.dst(), dst_flag);
}

TEST_F(GetterSetterTests, TestOperatorsBasic) {
    // Operator time_t

    EXPECT_EQ(time_t(d1) % (60 * 15), 1568648497 % (60 * 15));
    // Ugly fix, but should work in all situations.
    // The raw timestamp above was fetched when the author was in France during summer time = DST
    // ON. Anyway it's related to a timezone offset and there is no timezone (currently) with a
    // delta from UTC that requires more granularity than 15 minutes: see
    // https://en.wikipedia.org/wiki/List_of_UTC_time_offsets for reference.
    // So the fix above should make the test pass without weakening its validity too much.
    // TODO improve? get running machine's timezone offset and apply it to one of the values?

    // Operator struct tm
    struct tm md1 = tm(d1);
    EXPECT_EQ(md1.tm_year, year);
    EXPECT_EQ(md1.tm_mon, month);
    EXPECT_EQ(md1.tm_mday, monthday);
    EXPECT_EQ(md1.tm_hour, hour);
    EXPECT_EQ(md1.tm_min, minutes);
    EXPECT_EQ(md1.tm_sec, seconds);
    EXPECT_EQ(md1.tm_isdst, dst_flag);
    EXPECT_NE(md1.tm_yday, 0);
    EXPECT_NE(md1.tm_wday, 0);
}

TEST_F(GetterSetterTests, TestSetterDST) {
    EXPECT_EQ(d1.dst(DST_UNKNOWN), dst_flag); // unknown is generally not accepted by mktime.
    EXPECT_EQ(d1.dst(DST_ON), dst_flag);
    EXPECT_EQ(d1.dst(DST_OFF), dst_flag);

    try {
        d1.dst(123);
        FAIL() << "expected wrong time data";
    } catch (const DateError &de) {
        EXPECT_EQ(de.getErrorValue(), DateError_e::WRONG_TIME_DATA);
    }
    EXPECT_EQ(d1.dst(), dst_flag); // ensure stable state
}

TEST_F(GetterSetterTests, TestSetterSeconds) {
    EXPECT_EQ(d1.seconds(0), 0);
    EXPECT_EQ(d1.seconds(59), 59);
    EXPECT_EQ(d1.seconds(60), 0);

    try {
        d1.seconds(123);
        FAIL() << "expected wrong time data";
    } catch (const DateError &de) {
        EXPECT_EQ(de.getErrorValue(), DateError_e::WRONG_TIME_DATA);
    }
    try {
        d1.seconds(-2);
        FAIL() << "expected wrong time data";
    } catch (const DateError &de) {
        EXPECT_EQ(de.getErrorValue(), DateError_e::WRONG_TIME_DATA);
    }
    EXPECT_EQ(d1.seconds(), 0); // ensure stable state
}
