//
// Created by MartinF on 26/06/2022.
//

#if MF_UNIX

#    include <ctime>

#    include "MF/Timezones.hpp"

namespace MF
{
    namespace Timezones
    {
        std::chrono::seconds getTimezoneOffset() {
            const auto timezoneOffsetInSeconds = -timezone;
            return std::chrono::seconds(timezoneOffsetInSeconds);
        }

        std::chrono::seconds getDstOffset() {
            return std::chrono::seconds(daylight);
        }
    } // namespace Timezones
} // namespace MF

#endif
