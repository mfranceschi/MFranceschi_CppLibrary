//
// Created by mfran on 25/01/2020.
//

#include "ftime.h"
#include "Utils.h"
#include <windows.h>

static const unsigned int MICROSECONDS_IN_A_SECOND = (unsigned int)1e6;

int gettimeofday(timeval* t, void* tzp) {
    UNUSED(tzp);
    FILETIME ft;
    ULARGE_INTEGER uli;

    GetSystemTimeAsFileTime(&ft);
    uli = (ULARGE_INTEGER) {
            .LowPart = ft.dwLowDateTime,
            .HighPart = ft.dwHighDateTime
    };
    uli.QuadPart /= 10;

    t->tv_usec = uli.QuadPart % MICROSECONDS_IN_A_SECOND;
    t->tv_sec = uli.QuadPart / MICROSECONDS_IN_A_SECOND;
    return 0;
}

void usleep(useconds_t duration) {
    Sleep(duration / 1000);
}

int64_t timeval_difftime(const timeval* a, const timeval* b) {
    return (int64_t)(a->tv_sec - b->tv_sec) * MICROSECONDS_IN_A_SECOND + (int64_t)(a->tv_usec - b->tv_usec);
}
