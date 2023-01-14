//
// Created by MartinF on 03/07/2022.
//

#include "MF/Clocks.hpp"

namespace MF
{
    namespace Clocks
    {
        PauseableSteadyClock::PauseableSteadyClock() {
        }

        PauseableSteadyClock PauseableSteadyClock::newClock() {
            return PauseableSteadyClock();
        }

        void PauseableSteadyClock::pause() {
            inPause = true;
            lastPause = nowSteady();
        }

        void PauseableSteadyClock::unpause() {
            inPause = false;
            cumulated_offset = cumulated_offset + nowSteady() - lastPause;
        }

        PauseableSteadyClock::_underlying_clock::time_point PauseableSteadyClock::nowSteady() {
            return _underlying_clock::now();
        }

        PauseableSteadyClock::time_point PauseableSteadyClock::now() {
            return nowSteady() - cumulated_offset;
        }
        bool PauseableSteadyClock::isPaused() const {
            return inPause;
        }
    } // namespace Clocks
} // namespace MF
