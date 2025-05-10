//
// Created by MartinF on 13/04/2023.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
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

    std::unique_ptr<char[]> chars = std::make_unique<char[]>(expectedLength + 1);
    chars[expectedLength] = '\0';
    std::memset(chars.get(), 'a', sizeof(char) * expectedLength);

    ostream << chars.get();

    return EXIT_SUCCESS;
}
