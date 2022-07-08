//
// Created by MartinF on 26/06/2022.
//

#if MF_UNIX

#    include <sys/wait.h>

#    include <array>
#    include <cassert>
#    include <cstdio>
#    include <fstream>
#    include <memory>
#    include <regex>
#    include <sstream>

#    include "MF/CTime.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Timezones.hpp"

namespace MF
{
    namespace Timezones
    {
        static int getCurrentYear() {
            std::tm structTm{};
            MF::CTime::Localtime(structTm);
            return structTm.tm_year + 1900;
        }

        static std::string runCommandAndGetOutput(const std::string& command) {
            static constexpr std::size_t BUFFER_SIZE = 128;
            std::array<char, BUFFER_SIZE> buffer{};

            FILE* thePipeStream = popen(command.c_str(), "r");
            SystemErrors::throwCurrentSystemErrorIf(thePipeStream == nullptr);

            std::unique_ptr<FILE, decltype(&pclose)> pipe(thePipeStream, pclose);

            std::string result;
            while (fgets(buffer.data(), BUFFER_SIZE, pipe.get()) != nullptr) {
                result += buffer.data();
            }

            // The following instructions are extra precautions in case the child process hasn't
            // finished yet.
            int wstatus = 0;
            pid_t waitPidResult = waitpid(-1, &wstatus, 0);
            SystemErrors::throwCurrentSystemErrorIf(waitPidResult == -1);
            SystemErrors::throwCurrentSystemErrorIf(!WIFEXITED(wstatus));

            return result;
        }

        static std::string runZdump(const std::string& timezoneName) {
            const int currentYear = getCurrentYear();
            std::ostringstream oss;
            oss << "zdump -i -c " << currentYear << "," << currentYear + 1 << " " << timezoneName;
            std::string command = oss.str();

            return runCommandAndGetOutput(command);
        }



        static std::chrono::hours parseZdumpOutput(const std::string& timezoneName) {
            const std::string output = runZdump(timezoneName);
            std::istringstream iss(output);

            std::string buffer;

            // Skip the first three lines
            assert(std::getline(iss, buffer).good());
            assert(std::getline(iss, buffer).good());
            assert(std::getline(iss, buffer).good());

            assert(std::getline(iss, buffer).good());
            const std::string string1 = buffer;

            assert(std::getline(iss, buffer).good());
            const std::string string2 = buffer;

            struct ZdumpIntervalsOfInterest {
                std::string interval1, interval2;
            };
            const ZdumpIntervalsOfInterest intervalsOfInterest = {string1, string2};

            static const std::regex regex("^[0-9]+-[0-9]+-[0-9]+\t[0-9]+\t([+-]?)([0-9]+)\t.*$");
            static constexpr int SUBMATCH_FOR_SIGN = 1;
            static constexpr int SUBMATCH_FOR_VALUE = 2;

            std::smatch smatch1;
            std::smatch smatch2;

            if (std::regex_match(intervalsOfInterest.interval1, smatch1, regex) &&
                std::regex_match(intervalsOfInterest.interval2, smatch2, regex)) {
                auto sign1 = smatch1.str(SUBMATCH_FOR_SIGN);
                auto offset1 = smatch1.str(SUBMATCH_FOR_VALUE);

                auto sign2 = smatch2.str(SUBMATCH_FOR_SIGN);
                auto offset2 = smatch2.str(SUBMATCH_FOR_VALUE);

                auto numericOffset1 = (sign1 == "+" ? 1 : -1) * std::stoi(offset1);
                auto numericOffset2 = (sign2 == "+" ? 1 : -1) * std::stoi(offset2);

                auto difference = std::abs(numericOffset2 - numericOffset1);
                return std::chrono::hours(difference);
            }

            return std::chrono::hours(0);
        }

        std::chrono::seconds getTimezoneOffset() {
            const auto timezoneOffsetInSeconds = -timezone;
            return std::chrono::seconds(timezoneOffsetInSeconds);
        }

        std::chrono::seconds getDstOffset() {
            std::ifstream ifstream("/etc/timezone");
            MF::SystemErrors::throwCurrentSystemErrorIf(!ifstream.good());

            std::string timezoneName;
            std::getline(ifstream, timezoneName);
            const auto resultInHours = parseZdumpOutput(timezoneName);
            return std::chrono::duration_cast<std::chrono::seconds>(resultInHours);
        }
    } // namespace Timezones
} // namespace MF

#endif
