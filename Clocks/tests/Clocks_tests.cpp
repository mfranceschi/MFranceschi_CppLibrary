//
// Created by MartinF on 03/07/2022.
//

#include <thread>

#include "MF/Clocks.hpp"
#include "tests_data.hpp"

using namespace MF::Clocks;
using namespace std::chrono_literals;

using namespace std::chrono;

static ::testing::AssertionResult isDurationAcceptable(
    milliseconds duration, milliseconds minValue, milliseconds maxUpperValue) {
    bool isAcceptable = (duration >= minValue) && (duration <= minValue + maxUpperValue);

    if (isAcceptable) {
        return ::testing::AssertionSuccess();
    }

    return ::testing::AssertionFailure()
           << "Not acceptable duration: " << duration.count() << ". Expected a value in ["
           << minValue.count() << ", " << (minValue + maxUpperValue).count() << "]. ";
}

/**
 * We sleep 250ms for all clocks, and we sleep 150ms with paused clock.
 * Expected output: ~250ms elapsed for pauseable clock, ~400ms for steady clock.
 */
TEST(Clocks_PauseableSteadyClock, it_works) {
    auto myClock = PauseableSteadyClock::newClock();
    auto myTime1 = myClock.now();
    auto steadyTime1 = steady_clock::now();

    for (int i = 0; i < 3; i++) {
        myClock.pause();
        std::this_thread::sleep_for(50ms);
        myClock.unpause();
    }
    std::this_thread::sleep_for(250ms);

    auto myTime2 = myClock.now();
    auto steadyTime2 = steady_clock::now();

    EXPECT_TRUE(
        isDurationAcceptable(duration_cast<milliseconds>(steadyTime2 - steadyTime1), 400ms, 50ms))
        << "STEADY";
    EXPECT_TRUE(isDurationAcceptable(duration_cast<milliseconds>(myTime2 - myTime1), 250ms, 50ms))
        << "MY PAUSEABLE";
}

/**
 * We sleep 500ms for all clocks, and we sleep 1110ms with paused clock.
 * Expected output: ~500 elapsed for pauseable clock, ~1610ms for steady clock.
 */
TEST(Clocks_PauseableSteadyClock, it_works_longer_version) {
    auto myClock = PauseableSteadyClock::newClock();
    auto myTime1 = myClock.now();
    auto steadyTime1 = steady_clock::now();

    for (int i = 0; i < 10; i++) {
        myClock.pause();
        std::this_thread::sleep_for(111ms);
        myClock.unpause();
    }
    std::this_thread::sleep_for(500ms);

    auto myTime2 = myClock.now();
    auto steadyTime2 = steady_clock::now();

    EXPECT_TRUE(
        isDurationAcceptable(duration_cast<milliseconds>(steadyTime2 - steadyTime1), 1610ms, 150ms))
        << "STEADY";
    EXPECT_TRUE(isDurationAcceptable(duration_cast<milliseconds>(myTime2 - myTime1), 500ms, 50ms))
        << "MY PAUSEABLE";
}
