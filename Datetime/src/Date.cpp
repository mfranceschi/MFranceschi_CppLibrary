//---------- Implementation of class <Date> (file Date.cpp)

//---------------------------------------------------------------- INCLUDE
#include <cassert>
#include <chrono>
#include <cstring>
#include <functional>
#include <iomanip>
#include <mutex>
#include <sstream>

#include "MF/Datetime.hpp"

//-------------------------------------------------------------- Constants

// static functions
template <typename type = long long>
static constexpr int Sign(type nbr) {
    return !nbr ? 0 : nbr < static_cast<type>(0) ? -1 : +1;
}

namespace MF
{

    namespace Datetime
    {

        const char *Date::pattern = nullptr;

        int Date::tolerance = MS_MAX;
        char Date::msSepChar = NO_MS;

        Date Date::lastCallToNow = Date::Now();

        // Mutexes for static variables access.
        static std::mutex tolerance_mutex, pattern_mutex, msSep_mutex;

        // Template function for getter and setter of static variables access.
        template <typename T>
        static T set_static_value(
            T newValue, std::mutex &mutex_i, T &stored_value, std::function<bool(T)> isReadOnly) {
            mutex_i.lock();
            T returnValue = isReadOnly(newValue) ? stored_value : (stored_value = newValue);
            mutex_i.unlock();
            return returnValue;
        }

        /**
         * Attempts to put the "time_t" and "tm" values in a valid state.
         * In DEBUG, also assert the time_t is valid.
         * */
        static inline void assertInstanceIsValid(time_t &timet, struct tm &time) {
            constexpr time_t ERROR_VALUE = static_cast<time_t>(-1);
            timet = mktime(&time);
            // TODO don't C-assert but throw.
            assert(timet != ERROR_VALUE);
        }

        //----------------------------------------------------------------- PUBLIC

        //--------------------------------------------------------- Public methods

        const char *Date::str_pattern(const char *npattern) {
            return set_static_value<decltype(Date::pattern)>(
                npattern, pattern_mutex, Date::pattern, [](const char *newp) {
                    return newp != nullptr && strlen(newp) == 0;
                });
        }

        Date Date::Now() noexcept {
            using namespace std::chrono;
            using clock = system_clock;
            clock::time_point now_point = clock::now();
            Date newDate = Date::FromTime_t(clock::to_time_t(now_point));

            using mics = std::chrono::microseconds;
            mics::rep current_microseconds_count =
                duration_cast<mics>(now_point.time_since_epoch()).count();
            newDate.microseconds(
                static_cast<int>(current_microseconds_count - 1000000 * time_t(newDate)));
            return newDate;
        }

        int Date::Compare(const Date &d) const {
            return ::Sign(Timedelta(d).seconds);
        }

        Interval Date::Timedelta(const Date &param) const {
            std::time_t intervalInSeconds = static_cast<std::time_t>(difftime(timet, param.timet));
            decltype(microseconds_in) intervalInMicroseconds =
                microseconds_in - param.microseconds_in;

            std::uintmax_t totalInterval = (intervalInSeconds)*MS_MAX + (intervalInMicroseconds);
            return Interval(
                static_cast<std::time_t>(totalInterval % MS_MAX), intervalInMicroseconds);
        }

        // Getters and setters.

        int Date::seconds(int newvalue) {
            return quickSetter(newvalue, 0, 60, time.tm_sec);
        }

        int Date::minutes(int newvalue) {
            return quickSetter(newvalue, 0, 59, time.tm_min);
        }

        int Date::hours(int newvalue) {
            return quickSetter(newvalue, 0, 23, time.tm_hour);
        }

        int Date::day_month(int newvalue) {
            if (!newvalue) newvalue = -2;
            return quickSetter(
                newvalue, 1, DaysInMonth(newvalue, time.tm_year - 1900), time.tm_mday);
        }

        int Date::month(int newvalue) {
            return quickSetter(newvalue, 0, 11, time.tm_mon);
        }

        int Date::year(int newvalue) {
            if (newvalue < MAX_YEAR && newvalue > -MAX_YEAR - 1) {
                time.tm_year = newvalue;
                assertInstanceIsValid(timet, time);
                return time.tm_year;
            } else
                throw DateError(
                    DateError_e::WRONG_TIME_DATA,
                    "Year is not between -1 and " + std::to_string(MAX_YEAR) + ".");
        } // TODO

        int Date::dst(int newvalue) {
            if (newvalue == DST_UNKNOWN || newvalue == DST_OFF || newvalue == DST_ON) {
                time.tm_isdst = newvalue;
                assertInstanceIsValid(timet, time);
                return time.tm_isdst;
            } else
                throw DateError(
                    DateError_e::WRONG_TIME_DATA,
                    "Error while setting the DST flag, value is invalid: " +
                        std::to_string(newvalue));
        }

        int Date::microseconds(int newms) {
            if (newms == -1)
                return microseconds_in;
            else
                return microseconds_in = newms;
        }

        int Date::ms_tolerance(int newms) {
            return set_static_value<int>(
                newms, tolerance_mutex, Date::tolerance, [](int ms) -> bool {
                    return ms == -1;
                });
        }

        char Date::ms_CharSep(char newsep) {
            return set_static_value<char>(newsep, msSep_mutex, Date::msSepChar, [](char newc) {
                return newc == '\r';
            });
        }

        //-------------------------------------------------- Operator overloadings
        bool Date::operator==(const Date &other) const {
            using temp_type = long;

            temp_type currentMicrosecondsTime = temp_type(timet) * MS_MAX + microseconds_in;
            temp_type otherMicrosecondsTime =
                temp_type(other.timet) * MS_MAX + other.microseconds_in;
            return std::abs(currentMicrosecondsTime - otherMicrosecondsTime) <=
                   temp_type(tolerance);
        }

        Date::operator std::string() const {
            if (pattern == nullptr) {
                throw DateError(DateError_e::NO_PATTERN, "Pattern string is nullptr.");
            }

            std::ostringstream oss;
            oss << std::put_time(&time, pattern);

            if (msSepChar != NO_MS) {
                oss << msSepChar << microseconds_in;
            }

            return oss.str();
        }

        //---------------------------------------------- Constructors - destructor

        Date::Date() : Date(Now()) {
        }

        Date::Date(const std::string &src, const char *npattern) {
            if (npattern == nullptr) {
                if (Date::pattern != nullptr) {
                    npattern = Date::pattern;
                } else {
                    throw DateError(
                        DateError_e::NO_PATTERN,
                        "Pattern string is nullptr and this is not recoverable.");
                }
            }

            std::istringstream iss(src);
            Date ans;
            iss >> std::get_time(&time, npattern);

            if ((timet = mktime(&time)) == time_t(-1)) {
                throw DateError(DateError_e::WRONG_STRING, "Wrong pattern string: " + src);
            }
        }

        Date::Date(
            int year,
            int month,
            int monthday,
            int hour,
            int minutes,
            int seconds,
            int dst_flag,
            int microseconds) {
            time.tm_year = year;
            time.tm_mon = month;
            time.tm_mday = monthday;
            time.tm_hour = hour;
            time.tm_min = minutes;
            time.tm_sec = seconds;
            time.tm_isdst = dst_flag;
            microseconds_in = microseconds;
            if ((timet = mktime(&time)) == time_t(-1)) {
                throw DateError(
                    DateError_e::WRONG_TIME_DATA, "Something is wrong with the given inputs!");
            }
            assertInstanceIsValid(timet, time);
        }

        Date::Date(tm tm1, int microseconds) : time(tm1), microseconds_in(microseconds) {
            assertInstanceIsValid(timet, time);
        }

        //---------------------------------------------------------------- PRIVATE

        //------------------------------------------------------ Protected methods

        int Date::quickSetter(int newvalue, int min, int max, int &field) {
            if (newvalue == -1) {
                return field;
            } else {
                if (newvalue >= min && newvalue <= max) {
                    field = newvalue;
                } else {
                    throw DateError(DateError_e::WRONG_TIME_DATA, "Internal error in QuickSetter");
                }

                assertInstanceIsValid(timet, time);
                return field;
            }
        }
    } // namespace Datetime
} // namespace MF
