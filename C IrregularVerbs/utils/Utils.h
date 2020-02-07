//
// Created by Martin on 16/01/2020.
//

#ifndef IRREGULARVERBS_UTILS_H
#define IRREGULARVERBS_UTILS_H

#include <stdarg.h>
#include "Strings.h"

#define EXIT_BECAUSE_FILE_FAILURE 2 /* exit with this if a file failure occurred. */
#define EXIT_BECAUSE_CURSES_ISSUE 3 /* exit with this if the curses lib could not run initscr */
#define EXIT_BECAUSE_UNEXPECTED 9 /* exit with this NEVER please */

#define UNUSED(a) ((void)(a))

/**
 * Simply reads the CSV file and adds every verb to the VerbsContainer.
 * The container shall be opened.
 */
void fill_verbs_container();

/**
 * Get the number of occurrences of a substring in a given bigger string.
 *
 * @param substring The substring to look for.
 * @param big The string to look in.
 * @return 0 or more occurrences.
 */
size_t count_occurrences_of_substring(STRING substring, STRING big);

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

/**
 * Returns the minimum or maximum of the given arguments.
 *
 * @param n Number of arguments **following this one**.
 * @param ... List of numbers to test.
 * @return The min/max of the arguments, or INT_MIN/INT_MAX if no arguments.
 */
int min_nbr_var(int n, ...);
int max_nbr_var(int n, ...);

/** Returns the maximum or number between a and b. */
int max_nbr(int a, int b);
int min_nbr(int a, int b);

#endif //IRREGULARVERBS_UTILS_H