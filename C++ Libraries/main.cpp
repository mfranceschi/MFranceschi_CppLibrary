//
// Created by mfran on 08/12/2019.
//

#include "main.h"

/* Instructions for memory leak check */
#if defined _WIN32 && defined _MSC_VER
#define I_Want_Win_Mem_Check
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // WIN32
#include <cstdlib>

#include <Windows.h>
#include <iostream>
#include <string>
#include "Shlwapi.h"
#include "Toolbox.hpp"
#include "File.hpp"
#include <io.h>
#include <fcntl.h>
#include <ctime>
#include <mutex>
#include <sys/stat.h>

#define _CRT_SECURE_NO_WARNINGS
using namespace std;

static const size_t NBR_ITER = 10 * 1000;
#ifdef _WIN32
    static File::filename_t file_L = MAKE_FILE_NAME ".." FILE_SEPARATOR "Google_tests" FILE_SEPARATOR "files" FILE_SEPARATOR "aom_v.scx";
#endif
static const char *file_s = R"(..\Google_tests\files\aom_v.scx)";
#define FUNC Toolbox::Timethis(NBR_ITER, [&]() mutable

void my_assert(bool condition, const char *str, int nbr = -1) {
    if (!condition) {
        if (nbr == -1)
            cout << str << endl;
        else
            cout << str << " : " << nbr << endl;
        exit(EXIT_FAILURE);
    }
}

int old_unix_main() {
    int iter = 99;
    File::filename_t the_file_name = MAKE_FILE_NAME "bonjour.txt";
    int nbr_lines = 1;

    if (false) {
        for (int j = 0; j < iter; ++j)
            for (int i = 0; i < iter; ++i) {
                const char *content = File::Read(the_file_name);
                my_assert(content != nullptr, "fichier pas ok");
                Toolbox::InCharArrayStream icas(content, File::Size(the_file_name));
                std::string buffer;
                getline(icas, buffer);
                int linecpt = 0;
                while (icas) {
                    getline(icas, buffer);
                    ++linecpt;
                }
                my_assert(linecpt == nbr_lines, "BaD LiNe CoUnT", linecpt);

                File::Read_Close(content);
            }
    }

    if (true) {
        const char *content = File::Read(the_file_name);
        my_assert(content != nullptr, "fichier pas ok");
        Toolbox::InCharArrayStream icas(content, File::Size(the_file_name));
        std::string buffer;
        getline(icas, buffer);
        int line_count = 0;
        while (icas) {
            getline(icas, buffer);
            ++line_count;
            cout << buffer << endl;
        }
        my_assert(line_count == nbr_lines, "BaD LiNe CoUnT", line_count);

        File::Read_Close(content);
    }

    if (false) {
        for (int i = 0; i < iter; ++i) {
            for (int j = 0; j < iter; ++j) {
                istringstream iss("bonjour");
                iss.get();
            }
        }
    }

    cout << "DONE" << endl;
    return EXIT_SUCCESS;
}

static void timingTimeThis() {
    cout << "The duration of the execution of 'TimeThis' itself, without anything to do, is: " << FUNC {
                                                                                                       }) << endl;
    cout << endl;
}

static void timingTheFileExistence() {
    cout << "Timing the file existence functions !" << endl;

    cout << "Time for stat: " << FUNC {
                                          struct stat d{};
                                          stat(file_s, &d);
                                      }) << endl;

#if defined _WIN32
    cout << "Time for PathFileExists: " << FUNC {
                                                    PathFileExists(file_L);
                                                }) << endl;

    cout << "Time for GetFileAttributes: " << FUNC {
                                                       DWORD attr = GetFileAttributes(file_L);
                                                   }) << endl;
#endif // WIN32

    cout << endl;
}

static void timingTheFileSize() {
    cout << "Timing the file size functions !" << endl;

    cout << "Time for fseek : " << FUNC {
#pragma warning(suppress : 4996)
                                            FILE *f = fopen(file_s, "r");
                                            fseek(f, 0, SEEK_END);
                                            ftell(f);
                                            fclose(f);
                                        }) << endl;

    cout << "Time for stat: " << FUNC {
                                          struct stat st{};
                                          stat(file_s, &st);
                                      }) << endl;

#if defined _WIN32
    cout << "Time for GetFileAttributesEx: " << FUNC {
                                                         WIN32_FILE_ATTRIBUTE_DATA fileInfo;
                                                         GetFileAttributesEx(file_L, GetFileExInfoStandard,
                                                                             (void *) &fileInfo);
                                                     }) << endl;

    cout << "Time for GetFileSizeEx: " << FUNC {
                                                   HANDLE file = CreateFile(file_L, GENERIC_READ, FILE_SHARE_WRITE,
                                                                            nullptr, OPEN_EXISTING, NULL, nullptr);
                                                   LARGE_INTEGER res;
                                                   GetFileSizeEx(file, &res);
                                                   CloseHandle(file);
                                               }) << endl;
#endif // WIN32

    cout << endl;
}

static void timingWchar_tConversion() {
    cout << "Timing char to wchar_t conversion functions!" << endl;
    const wchar_t *newptr = nullptr;

    cout << "Time for mbstowcs_s: " << FUNC {
                                                newptr = Toolbox::ToWchar_t(file_s);
                                            }) << endl;
    delete[] newptr;

    cout << "Time for wstring creation: " << FUNC {
                                                      std::string s = file_s;
                                                      newptr = std::wstring(s.cbegin(), s.cend()).c_str();
                                                  }) << endl;

    cout << endl;
}

static void timingFileReading() {
#pragma warning(disable : 4996)
    cout << "Timing functions for reading 5 chars in a file!" << endl;
    char *buffer = new char[6];

    cout << "Time for FILE* with fgets: " << FUNC {
                                                      FILE *file = fopen(file_s, "r");
                                                      fgets(buffer, 6, file);
                                                      fclose(file);
                                                  }) << endl;

    cout << "Time for FILE* with fgetc: " << FUNC {
                                                      FILE *file = fopen(file_s, "r");
                                                      for (int i = 0; i < 5; ++i)
                                                          buffer[i] = fgetc(file);
                                                      buffer[5] = '\0';
                                                      fclose(file);
                                                  }) << endl;

    cout << "Time for ifstream with getline: " << FUNC {
                                                           std::ifstream ifs(file_s);
                                                           ifs.getline(buffer, 6);
                                                           ifs.close();
                                                       }) << endl;

    cout << "Time for ifstream with get: " << FUNC {
                                                       std::ifstream ifs(file_s);
                                                       for (int i = 0; i < 5; ++i)
                                                           buffer[i] = ifs.get();
                                                       buffer[5] = '\0';
                                                       ifs.close();
                                                   }) << endl;

#if defined _MSC_VER
    cout << "Time for Windows syscalls: " << FUNC {
                                                      int file;
                                                      _sopen_s(&file, file_s, _O_RDONLY, _SH_DENYNO, _S_IREAD);
                                                      auto tmp = _read(file, buffer, 5);
                                                      _close(file);
                                                  }) << endl;
#endif // WIN32

    cout << endl;
#pragma warning(default : 4996)
}

static void timingCtimeFunctions() {
#pragma warning(disable : 4996)
    cout << "Timing <ctime> conversion functions between tm and time_t !" << endl;
    time_t timet = time(nullptr);
    tm tmt{};

    cout << "Time for localtime: " << FUNC {
                                               tmt = *localtime(&timet);
                                           }) << endl;

#if defined __STDC_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1
    cout << "Time for localtime_s: " << FUNC {
                                                 localtime_s(&tmt, &timet);
                                             }) << endl;

    cout << "Time for gmtime_s: " << FUNC {
                                              gmtime_s(&tmt, &timet);
                                          }) << endl;
#endif

    cout << "Time for mktime (with local [FR] time tm): " << FUNC {
                                                                      timet = mktime(&tmt);
                                                                  }) << endl;

    cout << "Time for gmtime: " << FUNC {
                                            tmt = *gmtime(&timet);
                                        }) << endl;

    cout << "Time for mktime (with GMT time tm): " << FUNC {
                                                               timet = mktime(&tmt);
                                                           }) << endl;

    cout << endl;
#pragma warning(default: 4996)
}

void CheckReadMemoryLeaks() {
#if defined _MSC_VER
    _CrtMemState states[3];
    _CrtMemCheckpoint(&states[0]);

    for (int i = 0; i < 25; ++i) {
        auto size = File::Size(file_L);
        const char *content = File::Read(file_L);
        Toolbox::InCharArrayStream isac(content, size);
        std::string strbuf;
        while (isac) getline(isac, strbuf);
        File::Read_Close(content);
    }
    _CrtMemCheckpoint(&states[1]);
    if (_CrtMemDifference(&states[2], &states[0], &states[1])) {
        _CrtMemDumpStatistics(&states[2]);
    }
#endif // WIN32
}


void DoAllTimings() {
    timingTimeThis();
    timingTheFileExistence();
    timingTheFileSize();
    timingWchar_tConversion();
    timingFileReading();
    timingCtimeFunctions();
    CheckReadMemoryLeaks();
}

int main() {
    wcout << L"Current working dir: " << File::GetCWD() << endl;
    DoAllTimings();
    Toolbox::PressAnyKeyToContinue();
    return EXIT_SUCCESS;
}