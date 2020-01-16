//
// Created by Martin on 16/01/2020.
//

#include <string.h>
#include "Strings.h"

static const STRING SEPARATOR = ",";

void makeMultiStrings(STRING input, MultiStrings* output) {
    /* TODO */
    output->length = 1;
    output->array = malloc(output->length * sizeof(STRING));
    output->array[0] = malloc(27);
}

void freeMultiStrings(MultiStrings* ms) {
    for (size_t i = 0; i < ms->length; i++) {
        free((void *) ms->array[i]);
    }
    free(ms->array);
    ms->length = 0;
}

bool isSubstringOfString(STRING potential, STRING toCompareTo) {
    char* result = strstr(toCompareTo, potential);
    return result && result != toCompareTo;
}

bool isSubstringOfMultiStrings(STRING potential, const MultiStrings* toCompareTo) {
    for (size_t i = 0; i < toCompareTo->length; i++) {
        if (isSubstringOfString(potential, toCompareTo->array[i])) {
            return true;
        }
    }
    return false;
}

bool matchesString(STRING potential, STRING toCompareTo) {
    return !strcmp(potential, toCompareTo);
}

bool matchesMultiStrings(STRING potential, const MultiStrings* toCompareTo) {
    for (size_t i = 0; i < toCompareTo->length; i++) {
        if (matchesString(potential, toCompareTo->array[i])) {
            return true;
        }
    }
    return false;
}

STRING makeStringFromMultiStrings(MultiStrings* input, STRING separator) {
    // equivalent to Python's str.join(iterable)
    return NULL;
}
