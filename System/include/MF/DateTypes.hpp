//
// Created by MartinF on 06/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_DATETYPES_HPP
#define MFRANCESCHI_CPPLIBRARIES_DATETYPES_HPP

#include "MF/DateError.hpp"

namespace MF {
    namespace DateTypes {

        /**
         * Immutable class representing a distance between two time points.
         */
        struct Interval_class {
            const std::time_t seconds;
            const std::uint32_t nanos;

            constexpr explicit Interval_class(std::time_t seconds = 0, std::uint32_t nanos = 0) : seconds(seconds),
                                                                                                  nanos(nanos) {
                // Note: we don't check for the value of 'nanos' to be < 1e9 to limit overhead.
            }

            constexpr explicit operator double() const { return double(seconds) + 1e9 / double(nanos); }

            constexpr explicit operator std::uint32_t() const { return seconds; }

            constexpr unsigned int getMicrosecondsPart() const { return nanos / (unsigned int) 1e3; }

            constexpr unsigned int getMillisecondsPart() const { return nanos / (unsigned int) 1e6; }

            constexpr unsigned int getSecondsPart() const { return seconds % 60; }

            constexpr unsigned int getMinutesPart() const { return (seconds / 60) % 60; }

            constexpr unsigned int getHoursPart() const { return (seconds / 3600) % 3600; }

            constexpr unsigned int getDaysPart() const { return seconds / 86400; }

            constexpr Interval_class operator+(const Interval_class &other) const {
                std::uint32_t newNanos = this->nanos + other.nanos;
                std::time_t newSeconds = this->seconds + other.seconds;
                if (newNanos > std::uint32_t(1e9)) {
                    newNanos -= std::uint32_t(1e9);
                    newSeconds++;
                }
                return Interval_class(newSeconds, newNanos);
            }

            constexpr bool operator==(const Interval_class &other) const {
                return this->nanos == other.nanos && this->seconds == other.seconds;
            }
        };

        using Interval_t = Interval_class;
        using TimezoneOffset_t = long;
    }
}

#endif //MFRANCESCHI_CPPLIBRARIES_DATETYPES_HPP
