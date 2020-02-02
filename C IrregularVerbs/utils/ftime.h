//
// Created by mfran on 25/01/2020.
//

#ifndef IRREGULARVERBS_FTIME_H
#define IRREGULARVERBS_FTIME_H

/**
 * ftime.h, or FAKE-<sys/time.h>.
 * This module simulates the behavior of the <sys/time.h> library, which is deprecated under MinGW.
 * Do not use is as a full replacement because the contents are customized to fit in IrregularVerbs needs.
 */

#include <stdint.h>

typedef unsigned long seconds_t;
typedef unsigned long useconds_t;

typedef struct timeval_s {
    seconds_t tv_sec;
    useconds_t tv_usec;
} timeval;

/**
 * Fills the given struct with the current time infos.
 *
 * @param t The struct to fill.
 * @param tzp Ignored, NULL is okay.
 * @return 0 (always)
 */
int gettimeofday(timeval* t, void* tzp);

/**
 * Sleeps.
 *
 * @param length Duration in microseconds (we only consider milliseconds).
 */
void usleep(useconds_t length);

/**
 * BONUS FUNCTION (not in <sys/time.h>).
 *
 * @param a
 * @param b
 * @return a-b in microseconds
 */
int64_t timeval_difftime(const timeval* a, const timeval* b);

#endif //IRREGULARVERBS_FTIME_H
