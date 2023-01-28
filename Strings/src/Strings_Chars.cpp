//
// Created by Utilisateur on 05/11/2022.
//

#include <chrono>
#include <cwctype>
#include <locale>

#include "../../Commons/include/MF/Commons.hpp"
#include "MF/Strings.hpp"

namespace MF
{
    namespace Strings
    {
        namespace Chars
        {
            class LocaleCache : Commons::NoCopy, Commons::NoMove {
               private:
                using clock_t = std::chrono::steady_clock;

                static LocaleCache instance;

                LocaleCache() = default;
                void fetch(const clock_t::time_point& theNow) {
                    cachedGlobalLocale = std::locale();
                    latestFetch = theNow;
                }

                std::locale cachedGlobalLocale;
                clock_t ::time_point latestFetch = clock_t ::now();

               public:
                static std::locale& getNow() {
                    return instance.cachedGlobalLocale;
                }

                static std::locale& getOrFetch() {
                    using namespace std::chrono_literals;

                    const clock_t::time_point theNow = clock_t ::now();
                    if (theNow - instance.latestFetch > 1s) {
                        instance.fetch(theNow);
                    }

                    return getNow();
                }
            };

            LocaleCache LocaleCache::instance{};

            bool isBlank(char value) {
                return std::isblank(value, LocaleCache::getOrFetch());
            }

            bool isBlank(wchar_t value) {
                return std::iswblank(value) != 0;
            }

            bool isSpace(char value) {
                return isspace(value) != 0;
            }

            bool isSpace(wchar_t value) {
                return iswspace(value) != 0;
            }

            char toUpper(char value) {
                return std::toupper(value);
            }

            wchar_t toUpper(wchar_t value) {
                return std::towupper(value);
            }

            char toLower(char value) {
                return std::tolower(value);
            }

            wchar_t toLower(wchar_t value) {
                return std::towlower(value);
            }
        } // namespace Chars
    } // namespace Strings
} // namespace MF
