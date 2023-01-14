//
// Created by MartinF on 14/01/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_CONSOLE_HPP
#define MFRANCESCHI_CPPLIBRARIES_CONSOLE_HPP

#include <regex>
#include <string>
#include <vector>

#include "MF/Optionals.hpp"

namespace MF
{
    namespace Console
    {
        struct ConsoleToolbox {
            static ConsoleToolbox& forStdout();
            static ConsoleToolbox& forStderr();

            /**
             * Waits until the user presses any key, and returns it. Outputs nothing.
             */
            char getNext();

            /**
             * Sets the colour of the given standard stream.
             * We don't check nor handle the "colour" value, it is on you.
             * It varies by OS, platform, console capabilities, etc.
             * TODO maybe delete? or use a few general params such as BOLD, etc.?
             */
            void setColour(int colour);

            /**
             * Similar to Unix's "read". Displays a text, appending "\n" at the end if missing,
             * and waits for the user to write any non-blank string.
             */
            std::string getUserAnswer(const std::string& question);

            /**
             * Similar to the above, except that the function won't return until the user inputs
             * anything that matches one of the regexes, or until maxAttempts is reached. The
             * returned string is the string that corresponds to the matched regex.
             *
             * If maxAttempts is reached because the user wrote an incorrect output too many times,
             * returns an empty optional.
             */
            Optionals::OptionalPtr<std::string> getUserChoice(
                const std::string& question,
                const std::vector<std::pair<std::regex, std::string>>& possibleAnswers,
                const std::string& incorrectInputMessage = "Incorrect input.",
                std::uint16_t maxAttempts = 0);

            void pressEnterToContinue() {
                char lastChar = '\0';
                while (lastChar != '\n') {
                    lastChar = getNext();
                }
            }

            bool getYesOrNo() {
                const std::pair<std::regex, std::string> YES_CHOICE =
                    std::make_pair(std::regex("^[Yy]"), "yes");

                const std::pair<std::regex, std::string> NO_CHOICE =
                    std::make_pair(std::regex("^[Nn]"), "no");

                const std::vector<std::pair<std::regex, std::string>> CHOICES{
                    YES_CHOICE, NO_CHOICE};

                return getUserChoice(std::string(), CHOICES)
                    ->map<bool>([&YES_CHOICE](const std::string& answer) {
                        return answer == YES_CHOICE.second;
                    })
                    ->getOrDefault(false);
            }

           private:
            enum class StdStream_e { STD_STREAM_OUT, STD_STREAM_ERR };

            ConsoleToolbox(std::ostream& stdStream, StdStream_e currentStdStream);

            std::ostream& stdStream;
            const StdStream_e currentStdStream;
        };
    } // namespace Console
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_CONSOLE_HPP
