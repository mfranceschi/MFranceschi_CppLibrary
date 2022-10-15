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
        void setSystemTz(const std::string& newValue, bool runTzSet) {
            MF::Environment::setEnv("TZ", newValue);
            if (runTzSet) {
                tzSet();
            }
        }

        std::string getSystemTz() {
            return MF::Environment::getEnv("TZ");
        }

        void tzSet() {
#if MF_WINDOWS
            _tzset();
#else
            tzset();
#endif
        }
    } // namespace Timezones
} // namespace MF
