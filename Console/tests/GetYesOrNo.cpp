//
// Created by MartinF on 20/01/2023.
//

#include <iostream>

#include "MF/Console.hpp"

using namespace MF::Console;

/**
 * Exit values:
 * - 0 = input was detected as "Yes".
 * - 1 = input was detected as "No".
 * - 2 = input was not parsed correctly in the given attempts.
 */
int main(int nargs, char** args) {
    ConsoleToolbox::GetUserChoiceParams params{};
    params.incorrectInputMessage = ""; // No output please.

    if (nargs == 2) {
        params.maxAttempts = std::atoi(args[1]);
    }

    const std::vector<std::pair<std::regex, std::string>> CHOICES{
        std::make_pair(std::regex("^[Yy]"), "yes"), std::make_pair(std::regex("^[Nn]"), "no")};

    params.possibleAnswers = CHOICES;

    return ConsoleToolbox::forStdout()
        .getUserChoice(params)
        ->map<int>([](const std::string& answer) {
            return (answer == "yes") ? EXIT_SUCCESS : EXIT_FAILURE;
        })
        ->getOrDefault(2);
}
