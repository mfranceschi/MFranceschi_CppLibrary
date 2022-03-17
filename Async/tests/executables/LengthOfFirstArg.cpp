/**
 * RETURNS THE LENGTH (in bytes) OF THE FIRST ARGUMENT.
 * EXAMPLE : "abcde" -> 5.
 */

#include <string>

int main(int argc, char** argv) {
    if (argc >= 2) {
        return std::string(argv[1]).size();
    } else {
        return 0;
    }
}
