/**
 * THIS FILE REPEATS ITS INPUT TO ITS OUTPUT AND APPENDS NEW LINE TO IT.
 * IT ALSO REPEATS EACH OF ITS ARGUMENTS TO THE ERROR STREAM
 * AND RETURNS THE NUMBER OF ARGUMENTS.
 * NOTE: THE FIRST ARGUMENT IS NOT REPEATED (i.e. the executable name).
 */

#include <cstdio>

#if MF_UNICODE
#    include <fcntl.h>
#    include <io.h>

int wmain(int argc, wchar_t* argv[]) {
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);

    wchar_t inputBuffer[200] = {0};
    fgetws(inputBuffer, 200, stdin);

    wprintf(L"%s\n", inputBuffer);

    for (int i = 1; i < argc; i++) {
        fwprintf(stderr, L"%d: %s\n", i, argv[i]);
    }
    return argc;
}

#else

int main(int argc, char** argv) {
    char inputBuffer[200] = {0};
    std::fgets(inputBuffer, 200, stdin);

    printf("%s\n", inputBuffer);

    for (int i = 1; i < argc; i++) {
        fprintf(stderr, "%d: %s\n", i, argv[i]);
    }
    return argc;
}

#endif
