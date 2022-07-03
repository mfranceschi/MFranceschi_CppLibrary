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
