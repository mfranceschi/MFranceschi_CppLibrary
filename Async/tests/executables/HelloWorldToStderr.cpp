//
// Created by mfran on 09/05/2020.
//

#include <iostream>

#if MF_UNICODE
#    include <fcntl.h>
#    include <io.h>
int main() {
    _setmode(_fileno(stderr), _O_WTEXT);
    std::wcerr << L"Hello, World!"; // no end of line
    return 0;
}

#else
int main() {
    std::cerr << "Hello, World!"; // no end of line
    return 0;
}
#endif
