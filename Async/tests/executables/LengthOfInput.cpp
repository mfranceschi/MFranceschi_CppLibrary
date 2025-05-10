/**
 * RETURNS THE LENGTH (in bytes) OF THE FIRST LINE IN INPUT STREAM.
 */

#include <iostream>
#include <string>

int main() {
    std::cout << "HEY THERE I AM STARTING!!" << std::endl << std::flush;
    std::string inputString;
    std::cin >> inputString;
    std::cout << "HEY THERE I AM ENDING!!" << std::endl << std::flush;
    return inputString.size();
}
