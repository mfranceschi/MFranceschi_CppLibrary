/**
 * RETURNS THE LENGTH (in bytes) OF THE FIRST LINE IN INPUT STREAM.
 */

#include <string>
#include <iostream>

int main() {
    std::string s;
    std::getline(std::cin, s);
    return s.size();
}
