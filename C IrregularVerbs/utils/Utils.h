//
// Created by Martin on 16/01/2020.
//

#ifndef IRREGULARVERBS_UTILS_H
#define IRREGULARVERBS_UTILS_H

#include <stdarg.h>
#include "../Strings.h"

#define EXIT_BECAUSE_FILE_FAILURE 2 /* exit with this if a file failure occurred. */
#define EXIT_BECAUSE_UNEXPECTED 3 /* exit with this NEVER please */

#define UNUSED(a) ((void)(a))

/**
 * Simply reads the CSV file and adds every verb to the VerbsContainer.
 * The container shall be opened.
 */
void fillVerbsContainer();

/**
 * Get the number of occurrences of a substring in a given bigger string.
 *
 * @param substring The substring to look for.
 * @param big The string to look in.
 * @return 0 or more occurrences.
 */
size_t countOccurrencesOfSubstring(STRING substring, STRING big);

/**
 * Runs the given function and waits (with a thread sleeping function)
 * until the given amount of milliseconds have elapsed since the beginning of "run_and_wait".
 * Not very precise because of processes etc.
 *
 * @param milliseconds Number of milliseconds to wait.
 * @param function The function to run.
 * @param ... The function's parameters.
 */
void run_and_wait ( unsigned int milliseconds, void(* function) (va_list), ...);

/** Returns the maximum number between a and b. */
#define max_nbr(a,b) ((a > b) ? (a) : (b))

#endif //IRREGULARVERBS_UTILS_H
