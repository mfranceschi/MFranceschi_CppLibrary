//
// Created by mfran on 09/05/2020.
//

#include <cstdlib>

int main () {
#if defined(_WIN32)
    std::system("echo %cd%");
#else
    std::system("pwd");
#endif
    return 0;
}