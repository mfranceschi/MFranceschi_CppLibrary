//
// Created by MartinF on 18/06/2022.
//

#if MF_WINDOWS

#    include "MF/LightWindows.hpp"
#    include "MF/Timezones.hpp"
#    include "MF/Windows.hpp"

namespace MF
{
    namespace Timezones
    {
        class TimeZoneInformation {
           public:
            TimeZoneInformation() : returnValue(GetTimeZoneInformation(&timeZoneInformation)) {
                if (returnValue == TIME_ZONE_ID_INVALID) {
                    throw Windows::GetCurrentSystemError();
                }
            }

            /** UTC = local time + bias */
            std::chrono::minutes bias() const {
                return std::chrono::minutes(timeZoneInformation.Bias);
            }

            /**
             * This value is added to the value of the Bias member to form the bias used during
             * daylight saving time.
             *
             * When checking at timezones myself, I found that the only values are 0, -30, -60.
             * Note: the StandardBias is always zero, so we're not interested in it.
             * */
            std::chrono::minutes dstBias() const {
                return std::chrono::minutes(timeZoneInformation.DaylightBias);
            }

           private:
            TIME_ZONE_INFORMATION timeZoneInformation{};
            DWORD returnValue;
        };

        std::chrono::seconds getTimezoneOffset() {
            const TimeZoneInformation timeZoneInformation;

            return std::chrono::duration_cast<std::chrono::seconds>(timeZoneInformation.bias());
        }

        std::chrono::seconds getDstOffset() {
            const TimeZoneInformation timeZoneInformation;

            return std::chrono::duration_cast<std::chrono::seconds>(timeZoneInformation.dstBias());
        }
    } // namespace Timezones
} // namespace MF

#endif
