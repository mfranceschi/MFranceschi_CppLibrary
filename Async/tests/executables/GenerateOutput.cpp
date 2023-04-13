//
// Created by MartinF on 13/04/2023.
//

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

void throwInvalidInput() {
    throw std::runtime_error(
        "Expected 3 arguments: length of output in chars, error stream to write to. "
        "Example: '20 out'");
}

int main(int argc, char** argv) {
    if (argc != 3) {
        throwInvalidInput();
    }

    const int expectedLength = stoi(std::string(argv[1]));

    const std::string stream = argv[2];
    std::ostream& ostream = stream == "out"   ? std::cout
                            : stream == "err" ? std::cerr
                                              : (throwInvalidInput(), std::cout);

    srand(time(nullptr));
    const char charToWrite = static_cast<char>('A' + rand() % ('Z' - 'A'));
    for (auto i = 0; i < expectedLength; i++) {
        ostream << charToWrite;
    }
    return EXIT_SUCCESS;
}
