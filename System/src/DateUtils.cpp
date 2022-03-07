//
// Created by MartinF on 07/03/2022.
//

#include <ctime>

#if MF_OS_IS_WINDOWS

#   include "MF/LightWindows.hpp"

#endif

#include "MF/DateUtils.hpp"

namespace MF {
    namespace DateUtils {

        bool Localtime(const time_t &src, struct tm &dest) {
#if MF_OS_IS_WINDOWS
            /* Win32-style Localtime_S is available. */
            return !localtime_s(&dest, &src);

#elif defined __STDC_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1
            /* Classic Localtime_S is available. */
        return localtime_s(&src, &dest);
#else
        /* Use POSIX-style Localtime_R. */
        return localtime_r(&src, &dest);

        /*
         * To use basic "localtime", we should use mutexes to ensure there are no conflicts.
         * I am not implementing it here.
         */
#endif
        }

        bool Gmtime(const time_t &src, struct tm &dest) {
#if MF_OS_IS_WINDOWS
            /* Win32-style Gmtime_S is available. */
            return !gmtime_s(&dest, &src);

#elif defined __STDC_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1
            /* Classic Gmtime_S is available. */
        return gmtime_s(&src, &dest);
#else
        /* Use POSIX-style Gmtime_R. */
        return gmtime_r(&src, &dest);

        /*
         * To use basic "gmtime", we should use mutexes to ensure there are no conflicts.
         * I am not implementing it here.
         */
#endif
        }

        long Timezone() {
#if MF_OS_IS_WINDOWS
            DYNAMIC_TIME_ZONE_INFORMATION dynamicTimeZoneInformation;
            GetDynamicTimeZoneInformation(&dynamicTimeZoneInformation);
            return long(dynamicTimeZoneInformation.Bias);
#else
            return timezone;
#endif
        }
    }
}
