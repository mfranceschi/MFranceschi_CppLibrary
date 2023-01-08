//
// Created by MartinF on 03/07/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_CTIME_HPP
#define MFRANCESCHI_CPPLIBRARIES_CTIME_HPP

#include <chrono>

namespace MF
{
    namespace Clocks
    {
        struct PauseableSteadyClock;
        struct pauseable_time_point;

        struct PauseableSteadyClock {
            using rep = std::chrono::steady_clock::rep;
            using period = std::chrono::steady_clock::period;
            using duration = std::chrono::steady_clock::duration;
            using time_point = std::chrono::steady_clock::time_point;

            static PauseableSteadyClock newClock();

            void pause();
            void unpause();

            bool isPaused() const;

            time_point now();

           private:
            PauseableSteadyClock();

            static std::chrono::steady_clock::time_point nowSteady();

            bool inPause = false;
            std::chrono::steady_clock::time_point lastPause;
            duration cumulated_offset = duration::zero();
        };
    } // namespace Clocks
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_CTIME_HPP
