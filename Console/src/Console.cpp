//
// Created by MartinF on 14/01/2023.
//

#include "MF/Console.hpp"

#include <iostream>

#include "MF/Strings.hpp"

namespace MF
{
    namespace Console
    {
        static void printIfNonEmptyWithNewline(std::ostream& stdStream, const std::string& text) {
            if (!text.empty()) {
                stdStream << text;
                if (!Strings::endsWith(text, '\n')) {
                    stdStream << "\n";
                }
            }
        }

        static std::string getNonBlankInput() {
            std::string input;
            while (Strings::isBlank(input)) {
                std::cin >> input;
                input = Strings::strip(input);
            }
            return input;
        }

        std::string ConsoleToolbox::getUserAnswer(const std::string& question) {
            printIfNonEmptyWithNewline(stdStream, question);

            return getNonBlankInput();
        }

        Optionals::OptionalPtr<std::string> ConsoleToolbox::getUserChoice(
            const GetUserChoiceParams& params) {
            printIfNonEmptyWithNewline(stdStream, params.question);

            std::string input;
            std::uint16_t attempts = 0;

            while (attempts < params.maxAttempts) {
                input = getNonBlankInput();

                // Look for a matching regex.
                for (const auto& pair : params.possibleAnswers) {
                    if (std::regex_search(
                            input, pair.first, std::regex_constants::match_continuous)) {
                        return Optionals::ofLvalue(pair.second);
                    }
                }

                printIfNonEmptyWithNewline(stdStream, params.incorrectInputMessage);
                attempts++;
            }

            return Optionals::empty<std::string>();
        }

        ConsoleToolbox::ConsoleToolbox(std::ostream& stdStream, StdStream_e currentStdStream)
            : stdStream(stdStream), currentStdStream(currentStdStream) {
        }

        ConsoleToolbox& ConsoleToolbox::forStdout() {
            static ConsoleToolbox instance(std::cout, StdStream_e::STD_STREAM_OUT);
            return instance;
        }

        ConsoleToolbox& ConsoleToolbox::forStderr() {
            static ConsoleToolbox instance(std::cerr, StdStream_e::STD_STREAM_ERR);
            return instance;
        }
    } // namespace Console
} // namespace MF
