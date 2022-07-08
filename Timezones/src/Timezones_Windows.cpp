//
// Created by MartinF on 18/06/2022.
//

#if MF_WINDOWS

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Timezones.hpp"

namespace MF
{
    namespace Timezones
    {
        class TimeZoneInformation {
           public:
            TimeZoneInformation() : returnValue(GetTimeZoneInformation(&timeZoneInformation)) {
                MF::SystemErrors::throwCurrentSystemErrorIf(returnValue == TIME_ZONE_ID_INVALID);
            }

            /**
             * UTC = local time + bias
             * NOTE: this seems to not work well at all.
             */
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
            SYSTEMTIME systemTime;
            SYSTEMTIME localTime;
            GetSystemTime(&systemTime);
            GetLocalTime(&localTime);

            FILETIME stf;
            FILETIME ltf;
            SystemTimeToFileTime(&systemTime, &stf);
            SystemTimeToFileTime(&localTime, &ltf);

            ULARGE_INTEGER ust{stf.dwLowDateTime, stf.dwHighDateTime};
            ULARGE_INTEGER ult{ltf.dwLowDateTime, ltf.dwHighDateTime};

            unsigned long long result = ult.QuadPart - ust.QuadPart;
            std::chrono::microseconds resultInMicroseconds(result / 10);

            return std::chrono::duration_cast<std::chrono::seconds>(resultInMicroseconds);
        }

        std::chrono::seconds getDstOffset() {
            const TimeZoneInformation timeZoneInformation;

            const auto dstBiasInMinutes = timeZoneInformation.dstBias() * -1;

            return std::chrono::duration_cast<std::chrono::seconds>(dstBiasInMinutes);
        }
    } // namespace Timezones
} // namespace MF

#endif
