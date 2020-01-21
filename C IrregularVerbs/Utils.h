//
// Created by Martin on 16/01/2020.
//

#ifndef IRREGULARVERBS_UTILS_H
#define IRREGULARVERBS_UTILS_H

#include "Strings.h"

#define EXIT_BECAUSE_FILE_FAILURE 2

void fillVerbsContainer();
size_t countOccurrencesOfSubstring(STRING substring, STRING big);
#define max_nbr(a,b) ((a > b) ? (a) : (b))

#endif //IRREGULARVERBS_UTILS_H
