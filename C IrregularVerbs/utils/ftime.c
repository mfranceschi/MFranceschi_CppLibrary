//
// Created by Martin on 25/01/2020.
//

#include "ftime.h"

static const unsigned int MICROSECONDS_IN_A_SECOND = (unsigned int)1e6;

#ifdef _WIN32

#include "Utils.h"
#include <windows.h>


int gettimeofday(timeval* t, void* tzp) {
    UNUSED(tzp);
    FILETIME ft;
    ULARGE_INTEGER uli;

    GetSystemTimeAsFileTime(&ft);
    uli = (ULARGE_INTEGER) {
            .LowPart = ft.dwLowDateTime,
            .HighPart = ft.dwHighDateTime
    };
    uli.QuadPart /= 10; // we just consider microseconds, not 100 nanoseconds.

    *t = (timeval) {
        .tv_usec = uli.QuadPart % MICROSECONDS_IN_A_SECOND,
        .tv_sec = uli.QuadPart / MICROSECONDS_IN_A_SECOND
    };
    return 0;
}

void usleep(useconds_t duration) {
    Sleep(duration / 1000);
}

#endif

int64_t timeval_difftime(const timeval* a, const timeval* b) {
    return (int64_t)(a->tv_sec - b->tv_sec) * MICROSECONDS_IN_A_SECOND + (int64_t)(a->tv_usec - b->tv_usec);
}
