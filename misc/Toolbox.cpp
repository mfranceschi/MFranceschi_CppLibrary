//---------- Implementation of module <Toolbox> (file Toolbox.cpp)

//--------------------------------------------------------------- Includes

#include "MF/Toolbox.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>

#if defined _WIN32 && defined _MSC_VER
#    define _CRTDBG_MAP_ALLOC
#    include <crtdbg.h>
#endif // WIN32

namespace Toolbox
{
    /////////////////////////////////////////////////////////////////  PRIVATE

    //-------------------------------------------------------------- Constants

    //------------------------------------------------------------------ Types

    //------------------------------------------------------- Static variables

    //------------------------------------------------------ Private functions

    //////////////////////////////////////////////////////////////////  PUBLIC
    //------------------------------------------------------- Public functions

    double TimeThis(size_t iter, const std::function<void(void)>& func) {
        using namespace std::chrono;
        high_resolution_clock::time_point beginning = high_resolution_clock::now();
        for (size_t i = 0; i < iter; i++) {
            func();
        }
        return (duration<double>(high_resolution_clock::now() - beginning).count()) / double(iter);
    }

    wchar_t* ToWchar_t(const char* source) {
        size_t length = strlen(source);
        auto* destination = new wchar_t[length + 1];
        size_t retValue = 0;

#if defined _MSC_VER || \
    (defined __STDC_LIB_EXT1__ && defined __STDC_WANT_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1)
        mbstowcs_s(&retValue, destination, length + 1, source, length);
#else
        retValue = mbstowcs(destination, source, length) - length;
#endif

        if (!retValue) {
            delete[] destination;
            destination = nullptr;
        }
        return destination;
    }

    InCharArrayStream::InCharArrayBuffer::InCharArrayBuffer(const char* content, size_t size)
        : std::stringbuf(std::ios_base::in) {
        char* p = const_cast<char*>(content);
        setg(p, p, p + size);
    }

    InCharArrayStream::InCharArrayStream(const char* content, size_t size) : icab(content, size) {
        set_rdbuf(&icab);
    }

    void PrettyAssertion(bool condition, const char* message) {
        using std::cout;
        using std::endl;

#ifdef NDEBUG
        if (!condition) {
            cout << "Fatal failure. ";
            if (message) {
                cout << "Details: " << message;
            }
            cout << endl;
            std::abort();
        }
#else
        (void)condition;
        (void)message;
#endif
    }

    void Win_CheckForMemoryLeaks(const std::function<void()>& func, size_t iterations) {
#ifdef _MSC_VER
        _CrtMemState states[3];
        _CrtMemCheckpoint(&states[0]);
#endif

        for (size_t i = 0; i < iterations; ++i) {
            func();
        }

#ifdef _MSC_VER
        _CrtMemCheckpoint(&states[1]);

        if (_CrtMemDifference(&states[2], &states[0], &states[1])) {
            _CrtMemDumpStatistics(&states[2]);
        }
#endif
    }
} // namespace Toolbox
