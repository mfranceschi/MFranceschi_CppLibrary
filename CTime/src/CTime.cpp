//
// Created by MartinF on 03/07/2022.
//

#define _XOPEN_SOURCE 500

#include "MF/CTime.hpp"

#include <array>

#if MF_WINDOWS
#    include <cstring>
#    include <iomanip>
#    include <sstream>
#endif

#if defined __STDC_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1
#    define HAS_SECURE_VERSIONS 1
#else
#    define HAS_SECURE_VERSIONS 0
#endif

#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
#    define HAS_RESTARTABLE_VERSIONS 1
#else
#    define HAS_RESTARTABLE_VERSIONS 0
#endif

#if _DEFAULT_SOURCE || _BSD_SOURCE || _SVID_SOURCE
#    define HAS_TIMEGM 1
#else
#    define HAS_TIMEGM 0
#endif

namespace MF
{
    namespace CTime
    {
        std::time_t Time() {
            return std::time(nullptr);
        }

        double Difftime(std::time_t time_end, std::time_t time_beg) {
            return std::difftime(time_end, time_beg);
        }

        std::time_t Mktime(std::tm &src) {
            return std::mktime(&src);
        }

        std::time_t LocaltimeReversed(std::tm &src) {
            return std::mktime(&src);
        }

        bool Localtime(struct std::tm &dest, const std::time_t &src) {
#if MF_WINDOWS
            // Win32-style secure version is always available.
            // Order of arguments is different from non-Win32 secure version.
            return !localtime_s(&dest, &src);
#elif HAS_SECURE_VERSIONS
            return localtime_s(&src, &dest);
#elif HAS_RESTARTABLE_VERSIONS
            return localtime_r(&src, &dest) != nullptr;
#else
#    error TODO implement using mutexes
#endif
        }

        bool Gmtime(struct std::tm &dest, const std::time_t &src) {
#if MF_WINDOWS
            // Win32-style secure version is always available.
            // Order of arguments is different from non-Win32 secure version.
            return !gmtime_s(&dest, &src);
#elif HAS_SECURE_VERSIONS
            return gmtime_s(&src, &dest);
#elif HAS_RESTARTABLE_VERSIONS
            return gmtime_r(&src, &dest) != nullptr;
#else
#    error TODO implement using mutexes
#endif
        }

        time_t GmtimeReversed(std::tm &src) {
#if MF_WINDOWS
            return _mkgmtime(&src);
#elif HAS_TIMEGM
            return timegm(&src);
#else
#    error TODO implement(?)
#endif
        }

        std::string Strftime(const std::tm &src, const char *format) {
            // According to the following link, it's 25 chars + '\0'.
            // https://en.cppreference.com/w/c/chrono/asctime
            static constexpr std::size_t BUFFER_SIZE_LIKE_CTIME = 25 + 1;

            const auto cLocale = std::setlocale(LC_ALL, nullptr);
            const auto cppLocale = std::locale();

            std::array<char, BUFFER_SIZE_LIKE_CTIME> buffer{0};
            size_t charsCount = std::strftime(buffer.data(), buffer.size(), format, &src);
            return charsCount ? buffer.data() : "";
        }
    } // namespace CTime
} // namespace MF
