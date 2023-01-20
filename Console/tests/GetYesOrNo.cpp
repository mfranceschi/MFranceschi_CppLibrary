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

    if (nargs == 2) {
        params.maxAttempts = std::atoi(args[1]);
    }

    const std::vector<std::pair<std::regex, std::string>> CHOICES{
        std::make_pair(std::regex("^[Yy]"), "yes"), std::make_pair(std::regex("^[Nn]"), "no")};

    params.possibleAnswers = CHOICES;

    return ConsoleToolbox::forStdout()
        .getUserChoice(params)
        ->map<bool>([](const std::string& answer) {
            return answer == "yes";
        })
        ->map<int>([](const bool& value) {
            return value ? 0 : 1;
        })
        ->getOrDefault(2);
}
