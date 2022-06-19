//
// Created by MartinF on 19/06/2022.
//

#include <memory>
#include <stdexcept>

#include "MF/Timezones.hpp"

namespace MF
{
    namespace Timezones
    {
        static void throwIfTzNotFound() {
            throw std::runtime_error("TZ environment variable not found.");
        }

        void setTz(const std::string& tzValue) {
#if MF_WINDOWS
            _putenv_s("TZ", tzValue.data());
            _tzset();
#else
            setenv("TZ", tzValue.data(), 1);
            tzset();
#endif
        }

        std::string getTz() {
#if MF_WINDOWS
            std::unique_ptr<char[]> tzVar;
            size_t requiredSize = 0;

            getenv_s(&requiredSize, nullptr, 0, "TZ");
            if (requiredSize == 0) {
                throwIfTzNotFound();
            }

            tzVar = std::make_unique<char[]>(requiredSize);
            getenv_s(&requiredSize, tzVar.get(), requiredSize, "LIB");
            return tzVar.get();
#else
            const char* tzVar = std::getenv("TZ");
            if (tzVar == nullptr) {
                throwIfTzNotFound();
            }
            return tzVar;
#endif
        }
    } // namespace Timezones
} // namespace MF
