//
// Created by Martin on 16/01/2020.
//

#ifndef IRREGULARVERBS_UTILS_H
#define IRREGULARVERBS_UTILS_H

#include "Strings.h"

#define EXIT_BECAUSE_FILE_FAILURE 2 /* exit with this if a file failure occurred. */
#define EXIT_BECAUSE_UNEXPECTED 3 /* exit with this NEVER please */

/**
 * Simply reads the CSV file and adds every verb to the VerbsContainer.
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

/** Returns the maximum number between a and b. */
#define max_nbr(a,b) ((a > b) ? (a) : (b))

#endif //IRREGULARVERBS_UTILS_H
