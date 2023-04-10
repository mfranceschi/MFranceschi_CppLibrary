/**
 * RETURNS THE LENGTH (in bytes) OF THE FIRST LINE IN INPUT STREAM.
 */

#include <iostream>
#include <string>

int main() {
    std::string inputString;
    std::getline(std::cin, inputString);
    return inputString.size();
}
