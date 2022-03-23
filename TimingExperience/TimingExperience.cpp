//
// Created by mfran on 25/12/2019.
//

#include "TimingExperience.hpp"

#include <ctime>
#include <functional>
#include <iostream>
#include <string>

#include "MFranceschi_CppLibrary.hpp"

#if MF_WINDOWS
#    include <Shlwapi.h>
#    include <fcntl.h>
#    include <io.h>
#else
#    include <sys/stat.h>
#endif

using std::cout;
using std::endl;

static constexpr const char *file_name =
    ".." FILE_SEPARATOR "test" FILE_SEPARATOR "files" FILE_SEPARATOR "aom_v.scx";
#ifdef _WIN32
static constexpr File::Filename_t file_L =
    MAKE_FILE_NAME ".." FILE_SEPARATOR "tests" FILE_SEPARATOR "files" FILE_SEPARATOR "aom_v.scx";
#endif

static inline double TimeWithRepetition(const std::function<void()> &func) {
    return Toolbox::TimeThis(TimingExperience::NUMBER_OF_ITERATIONS, func);
}

namespace TimingExperience
{
    void RunAll() {
        timingTimeThis();
        timingTheFileExistence();
        timingTheFileSize();
        timingWchar_tConversion();
        timingFileReading();
        timingCtimeFunctions();
    }

    void timingTimeThis() {
        cout << "The duration of the execution of 'TimeThis' itself, without anything to do, is: ";
        cout << TimeWithRepetition([]() {
        }) << endl;
        cout << endl;
    }

    void timingTheFileExistence() {
        cout << "Timing the file existence functions !" << endl;

        cout << "Time for stat: " << TimeWithRepetition([]() {
            struct stat d {};
            stat(file_name, &d);
        }) << endl;

#if defined _WIN32
        cout << "Time for PathFileExists: ";
        cout << TimeWithRepetition([]() {
            PathFileExists(file_L);
        }) << endl;

        cout << "Time for GetFileAttributes: ";
        cout << TimeWithRepetition([]() {
            DWORD attr = GetFileAttributes(file_L);
        }) << endl;
#endif // WIN32

        cout << endl;
    }

    void timingTheFileSize() {
        cout << "Timing the file size functions !" << endl;

        cout << "Time for fseek : ";
        cout << TimeWithRepetition([]() {
#pragma warning(suppress : 4996)
            FILE *f = fopen(file_name, "r");
            fseek(f, 0, SEEK_END);
            ftell(f);
            fclose(f);
        }) << endl;

        cout << "Time for stat: ";
        cout << TimeWithRepetition([]() {
            struct stat st {};
            stat(file_name, &st);
        }) << endl;

#if defined _WIN32
        cout << "Time for GetFileAttributesEx: ";
        cout << TimeWithRepetition([]() {
            WIN32_FILE_ATTRIBUTE_DATA fileInfo;
            GetFileAttributesEx(file_L, GetFileExInfoStandard, (void *)&fileInfo);
        }) << endl;

        cout << "Time for GetFileSizeEx: ";
        cout << TimeWithRepetition([]() {
            HANDLE file = CreateFile(
                file_L, GENERIC_READ, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
            LARGE_INTEGER res;
            GetFileSizeEx(file, &res);
            CloseHandle(file);
        }) << endl;
#endif // WIN32

        cout << endl;
    }

    void timingWchar_tConversion() {
        cout << "Timing char to wchar_t conversion functions!" << endl;
        const wchar_t *newptr = nullptr;

        cout << "Time for mbstowcs_s: ";
        cout << TimeWithRepetition([&newptr]() {
            newptr = Toolbox::ToWchar_t(file_name);
        }) << endl;
        delete[] newptr;

        cout << "Time for wstring creation: ";
        cout << TimeWithRepetition([&newptr]() {
            std::string s = file_name;
            newptr = std::wstring(s.cbegin(), s.cend()).c_str();
        }) << endl;

        cout << endl;
    }

    void timingFileReading() {
#pragma warning(disable : 4996)
        cout << "Timing functions for reading 5 chars in a file!" << endl;
        char *buffer = new char[6];

        cout << "Time for FILE* with fgets: ";
        cout << TimeWithRepetition([&buffer]() {
            FILE *file = fopen(file_name, "r");
            fgets(buffer, 6, file);
            fclose(file);
        }) << endl;

        cout << "Time for FILE* with fgetc: ";
        cout << TimeWithRepetition([&buffer]() {
            FILE *file = fopen(file_name, "r");
            for (int i = 0; i < 5; ++i) buffer[i] = static_cast<char>(fgetc(file));
            buffer[5] = '\0';
            fclose(file);
        }) << endl;

        cout << "Time for ifstream with getline: ";
        cout << TimeWithRepetition([&buffer]() {
            std::ifstream ifs(file_name);
            ifs.getline(buffer, 6);
            ifs.close();
        }) << endl;

        cout << "Time for ifstream with get: ";
        cout << TimeWithRepetition([&buffer]() {
            std::ifstream ifs(file_name);
            for (int i = 0; i < 5; ++i) buffer[i] = ifs.get();
            buffer[5] = '\0';
            ifs.close();
        }) << endl;

#if defined _MSC_VER
        cout << "Time for Windows syscalls: ";
        cout << TimeWithRepetition([&buffer]() {
            int file;
            _sopen_s(&file, file_name, _O_RDONLY, _SH_DENYNO, _S_IREAD);
            auto tmp = _read(file, buffer, 5);
            _close(file);
        }) << endl;
#endif // WIN32

        cout << endl;
#pragma warning(default : 4996)
    }

    void timingCtimeFunctions() {
#pragma warning(disable : 4996)
        cout << "Timing <ctime> conversion functions between tm and time_t !" << endl;
        time_t timet = time(nullptr);
        tm tmt{};

        cout << "Time for localtime: ";
        cout << TimeWithRepetition([&timet, &tmt]() {
            tmt = *localtime(&timet);
        }) << endl;

#if defined __STDC_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1
        cout << "Time for localtime_s: ";
        cout << TimeWithRepetition([&timet, &tmt]() {
            localtime_s(&tmt, &timet);
        }) << endl;

        cout << "Time for gmtime_s: ";
        cout << TimeWithRepetition([&timet, &tmt]() {
            gmtime_s(&tmt, &timet);
        }) << endl;
#endif

        cout << "Time for mktime (with local time tm): ";
        cout << TimeWithRepetition([&timet, &tmt]() {
            timet = mktime(&tmt);
        }) << endl;

        cout << "Time for gmtime: ";
        cout << TimeWithRepetition([&timet, &tmt]() {
            tmt = *gmtime(&timet);
        }) << endl;

        cout << "Time for mktime (with GMT time tm): ";
        cout << TimeWithRepetition([&timet, &tmt]() {
            timet = mktime(&tmt);
        }) << endl;
    }
} // namespace TimingExperience

int main() {
    TimingExperience::RunAll();
    return 0;
}