//
// Created by MartinF on 03/07/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_CTIME_HPP
#define MFRANCESCHI_CPPLIBRARIES_CTIME_HPP

#include <ctime> // for type definitions
#include <string>

namespace MF
{
    namespace CTime
    {
        /** Value corresponding to Asctime and Ctime, for use in Strftime. */
        static constexpr const char *DEFAULT_STRFTIME_FORMAT = "Www Mmm dd hh:mm:ss yyyy\n";

        std::time_t Time();
        double Difftime(std::time_t time_end, std::time_t time_beg);
        std::time_t Mktime(std::tm &src);

        /**
         * Thread-safe Localtime.
         * Default 'src' value uses the current UTC timestamp as source for the date.
         * Returns true on success and false on failre - use 'errno' on failure to investigate.
         */
        bool Localtime(struct std::tm &dest, const std::time_t &src = Time());

        /**
         * Tries to normalize 'src' by considering it as a local time,
         * and returns the corresponding 'time_t'.
         * Returns -1 on failure - no guarantee if 'src' has been affected or not in that case.
         *
         * Actually a wrapper for mktime. */
        std::time_t LocaltimeReversed(std::tm &src);

        /**
         * Thread-safe Gmtime.
         * Default 'src' value uses the current UTC timestamp as source for the date.
         * Returns true on success and false on failre - use 'errno' on failure to investigate.
         */
        bool Gmtime(struct std::tm &dest, const std::time_t &src = Time());

        /**
         * Tries to normalize 'src' by considering it as a UTC time,
         * and returns the corresponding 'time_t'.
         * Returns -1 on failure - no guarantee if 'src' has been affected or not in that case.
         */
        std::time_t GmtimeReversed(std::tm &src);

        /** Like strftime but returns a std::string. */
        std::string Strftime(const std::tm &src, const char *format = DEFAULT_STRFTIME_FORMAT);

        /** Like strptime. */
        bool Strptime(std::tm &dest, const char *src, const char *format = DEFAULT_STRFTIME_FORMAT);
    } // namespace CTime
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_CTIME_HPP
