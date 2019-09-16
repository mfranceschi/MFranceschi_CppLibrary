#include <gtest/gtest.h>
#include <chrono>
#include <cmath>
#include "pch.h"
#include "..//C++/Date.hpp"
#include "..//C++/Date.cpp"

namespace StaticMethodsTesting
{
	using clock = std::chrono::system_clock;

	TEST(IsLeapYear, EffectiveLeapYears)
	{
		EXPECT_TRUE(Date::IsLeapYear(0));
		EXPECT_TRUE(Date::IsLeapYear(2000));
		EXPECT_TRUE(Date::IsLeapYear(2004));
	}

	TEST(IsLeapYear, NotLeapYears)
	{
		EXPECT_FALSE(Date::IsLeapYear(1));
		EXPECT_FALSE(Date::IsLeapYear(2));
		EXPECT_FALSE(Date::IsLeapYear(3));
		EXPECT_FALSE(Date::IsLeapYear(100));
		EXPECT_FALSE(Date::IsLeapYear(2100));
	}

	TEST(DaysInMonth, OtherMonths)
	{
		// Months of 31 days.
		EXPECT_EQ(Date::DaysInMonth(Date::JANUARY), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::MARCH), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::MAY), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::MAY), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::JULY), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::AUGUST), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::OCTOBER), 31);
		EXPECT_EQ(Date::DaysInMonth(Date::DECEMBER), 31);

		// Months of 30 days.
		EXPECT_EQ(Date::DaysInMonth(Date::APRIL), 30);
		EXPECT_EQ(Date::DaysInMonth(Date::JUNE), 30);
		EXPECT_EQ(Date::DaysInMonth(Date::SEPTEMBER), 30);
		EXPECT_EQ(Date::DaysInMonth(Date::NOVEMBER), 30);
	}

	TEST(DaysInMonth, WrongInput)
	{
		EXPECT_EQ(Date::DaysInMonth(-1), 0);
		EXPECT_EQ(Date::DaysInMonth(-2), 0);
		EXPECT_EQ(Date::DaysInMonth(12), 0);
		EXPECT_EQ(Date::DaysInMonth(123), 0);
	}

	TEST(DaysInMonth, February)
	{
		EXPECT_EQ(Date::DaysInMonth(Date::FEBRUARY, 0), 29); // Leap year
		EXPECT_EQ(Date::DaysInMonth(Date::FEBRUARY, 1), 28); // Non-leap year
	}

	TEST(Now_Timestamp, FullTest)
	{
		time_t now_to_check = Date::Now_Timestamp();
		time_t now_from_chrono = clock::to_time_t(clock::now());
		time_t distance = now_from_chrono - now_to_check;
		if (distance < 0) distance *= -1;
		EXPECT_TRUE(distance <= 1);
	}
}

TEST_F(GetterSetterTests, TestGetters)
{
	EXPECT_EQ(d1.year(), year);
	EXPECT_EQ(d1.month(), month);
	EXPECT_EQ(d1.day_month(), monthday);
	EXPECT_EQ(d1.hours(), hour);
	EXPECT_EQ(d1.minutes(), minutes);
	EXPECT_EQ(d1.seconds(), seconds);
	EXPECT_EQ(d1.dst(), dst_flag);
}

TEST_F(GetterSetterTests, TestOperatorsBasic)
{
	// Operator time_t
	EXPECT_EQ(time_t(d1), 1568648497);

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

TEST_F(GetterSetterTests, TestSetterDST)
{
	EXPECT_EQ(d1.dst(Date::DST_UNKNOWN), dst_flag); // unknown is generally not accepted by mktime.
	EXPECT_EQ(d1.dst(Date::DST_ON), dst_flag);
	EXPECT_EQ(d1.dst(Date::DST_OFF), dst_flag);

	try {
		d1.dst(123);
		FAIL() << "expected wrong time data";
	}
	catch (DateError de) {
		EXPECT_EQ(de, DateError::WRONG_TIME_DATA);
	}
	EXPECT_EQ(d1.dst(), dst_flag); // ensure stable state
}

TEST_F(GetterSetterTests, TestSetterSeconds)
{
	EXPECT_EQ(d1.seconds(0), 0);
	EXPECT_EQ(d1.seconds(59), 59);
	EXPECT_EQ(d1.seconds(60), 0);
	
	try {
		d1.seconds(123);
		FAIL() << "expected wrong time data";
	}
	catch (DateError de) {
		EXPECT_EQ(de, DateError::WRONG_TIME_DATA);
	}
	try {
		d1.seconds(-5);
		FAIL() << "expected wrong time data";
	}
	catch (DateError de) {
		EXPECT_EQ(de, DateError::WRONG_TIME_DATA);
	}
	EXPECT_EQ(d1.seconds(), 0); // ensure stable state
}

