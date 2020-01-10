//
// Created by mfran on 25/12/2019.
//

#include <cstring>
#include <list>
#include <mutex>
#include "ArgumentsParser.hpp"

const ArgumentsParser &ArgumentsParser::parse(int nargs, const char **the_args) {
    std::list<std::pair<int, int>> listOfMultiItemsArguments;
    computeMultiItemsArguments(1, nargs, the_args, listOfMultiItemsArguments);
    _getArgumentsAsIntegers(1, nargs, the_args, listOfMultiItemsArguments);
    parseInternal(1, nargs, the_args);
    return *this;
}

ArgumentsParser::~ArgumentsParser() {
    free();
}

void ArgumentsParser::free() {
    for (const auto& argument : results) {
        // TODO
    }
    results.clear();
}

void ArgumentsParser::parseInternal(int begin, int end, const char **the_args) {
    // TODO
}

void
ArgumentsParser::_getArgumentsAsIntegers(int begin, int end, const char **the_args, const std::list<std::pair<int, int>> &lomia) {
    // Simple, lazy test of the parser.
    // Converts to zero on failure.

    auto lastMIA = lomia.cbegin();
    for (int i = begin; i < end; i++) {
        if (lastMIA->first == i) {
            // TODO do not skip

            lastMIA++;
        } else {
            int nbr = static_cast<int>(std::strtol(the_args[i], nullptr, 0));
            if (errno == ERANGE) {
                nbr = 0;
            }

            Argument arg;
            arg.present=true;
            arg.valueType = ValueType::INTEGER;
            arg.integer_v = nbr;
            results.insert({the_args[i], arg});
        }
    }
}

void ArgumentsParser::computeMultiItemsArguments(int begin, int end, const char **the_args,
                                                 std::list<std::pair<int, int>> &lomia) {
    // If some arguments are spread over multiple elements of "the_args",
    // we note their beginning and end indexes.
    // We then call them "Multi-Item argument" because there are several items from "the_args" to consider.

    std::pair<int, int> currentMultiItemArgument(-1, -1);

    for (int i = begin; i < end; i++) {
        size_t len = std::strlen(the_args[i]);
        if (len > 0) {

            // If the string begins with a ", we start a new Multi-Item argument if none is currently opened.
            if (the_args[i][0] == '"') {
                if (currentMultiItemArgument.first == -1) {
                    currentMultiItemArgument.first = i;
                } else {
                    throw UnfinishedMultiItemsArgument(currentMultiItemArgument.first);
                }
            }

            // If the string ends with a ", we end the currently opened Multi-Item argument if there is one.
            if (the_args[i][len - 1] == '"') {
                if (currentMultiItemArgument.first == -1) {
                    throw InvalidMultiItemsArgument(i);
                } else {
                    currentMultiItemArgument.second = i;
                    lomia.push_back(currentMultiItemArgument);
                    currentMultiItemArgument = {-1, -1};
                }
            }
        }
    }

    if (currentMultiItemArgument.first != -1) {
        throw UnfinishedMultiItemsArgument(currentMultiItemArgument.first);
    }
}

const char *UnfinishedMultiItemsArgument::what() const noexcept {
    constexpr static const char* PATTERN = "A multi item argument was seen as not finished. It began at index %d.";
    char* buffer = new char[std::strlen(PATTERN) + 2];
    sprintf(buffer, PATTERN, indexOfProblematicItem);
    return buffer;
}

const char *InvalidMultiItemsArgument::what() const noexcept {
    constexpr static const char* PATTERN = "A multi item argument was seen as invalid. It was seen at index %d.";
    char* buffer = new char[std::strlen(PATTERN) + 2];
    sprintf(buffer, PATTERN, indexOfProblematicItem);
    return buffer;
}
