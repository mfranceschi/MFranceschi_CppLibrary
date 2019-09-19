//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"
#include "..//C++/Date.hpp"

class GetterSetterTests : public ::testing::Test
{
protected:
	Date d1;
	Date d1_original;
	const static int year = 2019 - 1900;
	const static int month = Date::SEPTEMBER;
	const static int monthday = 16;
	const static int hour = 17;
	const static int minutes = 41;
	const static int seconds = 37;
	const static int dst_flag = Date::DST_ON;

	inline void SetUp() override;
};

void GetterSetterTests::SetUp() {
	d1 = Date(year, month, monthday, hour, minutes, seconds, dst_flag);
	d1_original = d1;
}
