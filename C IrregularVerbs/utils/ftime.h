//
// Created by Martin on 25/01/2020.
//

#ifndef IRREGULARVERBS_FTIME_H
#define IRREGULARVERBS_FTIME_H

/**
 * FAKE-<sys/time.h>.
 *
 * Two cases: (we choose by checking if _WIN32 macro is defined)
 * - If we are running a non-Windows-dependent compiler (so neither MinGW, neither MSVC) then that files just includes
 *   the regular <sys/time.h> and adds one utility function.
 * - Else we define ourselves two functions (same declaration as in <sys/time.h>) and add a new one.
 *   In that case, functions will implement a behavior for microseconds but it will not be any useful below the
 *   milliseconds range.
 */

#include <stdint.h>

#ifndef _WIN32

// We just need to include the file that usually works!
#include<sys/time.h>

#else

typedef unsigned long seconds_t;
typedef unsigned long useconds_t;

typedef struct timeval_s {
    seconds_t tv_sec;
    useconds_t tv_usec;
} timeval;

int gettimeofday(timeval* t, void* tzp);
void usleep(useconds_t length);

#endif

/**
 * BONUS FUNCTION (not in <sys/time.h>).
 *
 * @param a
 * @param b
 * @return a-b in microseconds
 */
int64_t timeval_difftime(const timeval* a, const timeval* b);

#endif //IRREGULARVERBS_FTIME_H
