/**
 * RETURNS THE LENGTH (in bytes) OF THE FIRST LINE IN INPUT STREAM.
 */

#include <iostream>

int main() {
    /*
    std::string line;
    size_t total_length = 0;
    while (std::getline(std::cin, line, static_cast<char>(EOF)).good()) {
        std::cout << "Line=" << line << std::endl;
        total_length += line.size();
    }
    if (std::cin.eof()) {
        std::cout << "End of file reached." << std::endl;
    } else if (std::cin.fail()) {
        std::cout << "Input stream failed." << std::endl;
    } else {
        std::cout << "Input stream is still good." << std::endl;
    }
    std::cout << total_length << std::endl;
    return static_cast<int>(total_length);
     */

    /*
    std::string std_input;
    std::stringstream stream;
    while (getline(std::cin, std_input).good()) {
        stream << std_input << std::endl;
    }

    auto resultingString = stream.str();
    // Remove last endl from resulting string if it exists
    if (!resultingString.empty() && resultingString.back() == '\n') {
        resultingString.pop_back(); // Remove the last newline character
    }
    std::cout << "String stream content: " << resultingString
              << ", length: " << resultingString.length() << std::endl;
    return resultingString.length(); // Return the length of the input stream
     */

    std::cin.seekg(0, std::ios::end);
    return static_cast<int>(std::cin.tellg());
}
