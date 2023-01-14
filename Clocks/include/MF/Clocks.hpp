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
           private:
            using _underlying_clock = std::chrono::steady_clock;

           public:
            using rep = _underlying_clock::rep;
            using period = _underlying_clock::period;
            using duration = _underlying_clock::duration;
            using time_point = _underlying_clock::time_point;

            static PauseableSteadyClock newClock();

            void pause();
            void unpause();

            bool isPaused() const;

            time_point now();

           private:
            PauseableSteadyClock();

            static _underlying_clock::time_point nowSteady();

            bool inPause = false;
            _underlying_clock::time_point lastPause;
            duration cumulated_offset = duration::zero();
        };
    } // namespace Clocks
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_CTIME_HPP
