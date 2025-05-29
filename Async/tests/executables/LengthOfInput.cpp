/**
 * RETURNS THE LENGTH (in bytes) OF THE FIRST LINE IN INPUT STREAM.
 */

#include <iostream>

int main() {
    std::cin.seekg(0, std::ios::end);
    return static_cast<int>(std::cin.tellg());
}
