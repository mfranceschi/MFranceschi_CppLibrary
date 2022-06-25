//
// Created by MartinF on 19/06/2022.
//

#include <ctime>
#include <stdexcept>

#include "MF/Environment.hpp"
#include "MF/Timezones.hpp"

namespace MF
{
    namespace Timezones
    {
        static void throwIfTzNotFound() {
            throw std::runtime_error("TZ environment variable not found.");
        }

        void setSystemTz(const std::string& tzValue) {
            MF::Environment::setEnv("TZ", tzValue);
        }

        std::string getSystemTz() {
            return MF::Environment::getEnv("TZ");
        }

        void initSystemTz() {
#if MF_WINDOWS
            _tzset();
#else
            tzset();
#endif
        }
    } // namespace Timezones
} // namespace MF
