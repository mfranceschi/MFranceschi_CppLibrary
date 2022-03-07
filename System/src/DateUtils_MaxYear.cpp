//
// Created by MartinF on 07/03/2022.
//

#include <limits>
#include "MF/DateUtils.hpp"

namespace MF {
    namespace DateUtils {
#if MF_OS_IS_WINDOWS
#   pragma warning(disable: 4127 4309 4365)
#endif

        constexpr static int MaxYear() noexcept {
            using ULL_t = unsigned long long;
            constexpr ULL_t seconds_in_year(NBR_SECONDS_IN_DAY * 365.2425L);
            constexpr ULL_t possible_seconds(std::numeric_limits<time_t>::max());
            constexpr ULL_t max_year_with_time_t(possible_seconds / seconds_in_year);
            constexpr int max_year_with_struct_tm(std::numeric_limits<int>::max());

            if (max_year_with_time_t > max_year_with_struct_tm)
                return static_cast<int>(max_year_with_struct_tm);
            else
                return static_cast<int>(max_year_with_time_t);
        }

        const int MAX_YEAR = MaxYear();
#if MF_OS_IS_WINDOWS
#   pragma warning(default: 4127 4309 4365)
#endif
    }
}