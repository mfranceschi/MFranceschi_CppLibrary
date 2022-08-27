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
            static constexpr int YEAR_OFFSET_IN_STRUCT_TM = 1900;

            std::tm structTm{};
            MF::CTime::Localtime(structTm);
            return structTm.tm_year + YEAR_OFFSET_IN_STRUCT_TM;
        }

        static std::string runCommandAndGetOutput(const std::string& command) {
            static constexpr std::size_t BUFFER_SIZE = 128;
            std::array<char, BUFFER_SIZE> buffer{};

            FILE* thePipeStream = popen(command.c_str(), "r");
            SystemErrors::Errno::throwCurrentSystemErrorIf(thePipeStream == nullptr);

            std::unique_ptr<FILE, decltype(&pclose)> pipe(thePipeStream, pclose);

            std::string result;
            while (fgets(buffer.data(), BUFFER_SIZE, pipe.get()) != nullptr) {
                result += buffer.data();
            }

            // Ensure the child process terminated by exiting with success status.
            int wstatus = 0;
            pid_t waitPidResult = waitpid(-1, &wstatus, 0);
            SystemErrors::Errno::throwCurrentSystemErrorIf(waitPidResult == -1);

            bool wIfExited = WIFEXITED(wstatus);
            int wExitStatus = WEXITSTATUS(wstatus);
            if (!(wIfExited && (wExitStatus == EXIT_SUCCESS))) {
                std::ostringstream oss;
                oss << "Error from zdump. The 'wstatus' value is " << std::hex << wstatus << ". ";
                oss << "wIfExited returns " << std::boolalpha << wIfExited << ". ";
                if (wIfExited) {
                    oss << "The exit status is " << std::dec << wExitStatus << ". ";
                }
                throw std::runtime_error(oss.str());
            }

            return result;
        }

        static std::string runZdump(const std::string& timezoneName) {
            const int currentYear = getCurrentYear();
            std::ostringstream oss;
            oss << "zdump -i -c " << currentYear << "," << currentYear + 1 << " " << timezoneName;
            std::string command = oss.str();

            return runCommandAndGetOutput(command);
        }

        class ISWrapper {
           public:
            void skipLine() {
                getLineAndThrowIfNotGood();
            }

            const std::string& getLine() {
                getLineAndThrowIfNotGood();
                return buffer;
            }

           protected:
            ISWrapper() = default;

            void setStream(std::istream* pointer) {
                istream1 = pointer;
            }

           protected:
            std::string buffer;
            std::istream* istream1 = nullptr;

            virtual void getLineAndThrowIfNotGood() {
                std::getline(*istream1, buffer);
                if (!istream1->good()) {
                    throw std::runtime_error(
                        "Unexpected stream error - pos=" + std::to_string(istream1->tellg()));
                }
            }
        };

        class ISSWrapper : public ISWrapper {
           public:
            ISSWrapper(const std::string& input) : istringstream1(input) {
                this->setStream(&istringstream1);
            }

           private:
            std::istringstream istringstream1;

            void getLineAndThrowIfNotGood() override {
                std::getline(*istream1, buffer);
                if (!istream1->good()) {
                    throw std::runtime_error(
                        "Unexpected stream error - pos=" + std::to_string(istream1->tellg()) +
                        " - string was <<" + istringstream1.str() + ">>.");
                }
            }
        };

        class IFSWrapper : public ISWrapper {
           public:
            IFSWrapper(const std::string& file) : fileStream(file) {
                MF::SystemErrors::Errno::throwCurrentSystemErrorIf(!fileStream.good());
                this->setStream(&fileStream);
            }

           private:
            std::ifstream fileStream;
        };

        static std::chrono::hours parseZdumpOutput(const std::string& timezoneName) {
            const std::string output = runZdump(timezoneName);
            ISSWrapper issWrapper(output);

            // Skip the first three lines
            issWrapper.skipLine();
            issWrapper.skipLine();
            issWrapper.skipLine();

            const std::string& intervalLine1 = issWrapper.getLine();
            const std::string& intervalLine2 = issWrapper.getLine();

            static const std::regex regex("^[0-9]+-[0-9]+-[0-9]+\t[0-9]+\t([+-]?)([0-9]+)\t.*$");
            static constexpr int SUBMATCH_FOR_SIGN = 1;
            static constexpr int SUBMATCH_FOR_VALUE = 2;

            std::smatch stringMatch1;
            std::smatch stringMatch2;

            if (std::regex_match(intervalLine1, stringMatch1, regex) &&
                std::regex_match(intervalLine2, stringMatch2, regex)) {
                auto sign1 = stringMatch1.str(SUBMATCH_FOR_SIGN);
                auto offset1 = stringMatch1.str(SUBMATCH_FOR_VALUE);

                auto sign2 = stringMatch2.str(SUBMATCH_FOR_SIGN);
                auto offset2 = stringMatch2.str(SUBMATCH_FOR_VALUE);

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
            const auto timezoneName = getTimezoneName();
            if ((timezoneName.find("UTC") != std::string::npos) ||
                (timezoneName.find("UCT") != std::string::npos) ||
                (timezoneName.find("Universal") != std::string::npos)) {
                // All the timezones that are identical to UTC contain one of the above substring.
                // So if we have a timezone that is UTC-like then we can safely return zero offset.
                return std::chrono::seconds(0);
            }

            const auto resultInHours = parseZdumpOutput(getTimezoneName());
            return std::chrono::duration_cast<std::chrono::seconds>(resultInHours);
        }

        std::string getTimezoneName() {
            static const std::string fileName = "/etc/timezone";
            IFSWrapper fileStream(fileName);

            const std::string& timezoneName = fileStream.getLine();
            return timezoneName;
        }
    } // namespace Timezones
} // namespace MF

#endif
