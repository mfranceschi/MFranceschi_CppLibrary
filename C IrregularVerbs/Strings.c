//
// Created by Martin on 16/01/2020.
//

#include "utils/Utils.h"
#include <string.h>
#include "Strings.h"

static const STRING SEPARATOR = ",";
static const size_t LEN_SEPARATOR = 1; // str len(SEPARATOR)

MultiStrings* makeMultiStrings(STRING input) {
    MultiStrings* output = malloc(sizeof(MultiStrings));
    size_t length;
    WRITEABLE_STRING* array;
    const size_t input_length = strlen(input);

    if (input[0] == '\0') {
        // If empty string then special case (faster).
        length = 1;
        array = malloc(sizeof(STRING));
        array[0] = malloc(1);
        array[0][0] = '\0';
    } else {
        // Count how many separators = n
        // Allocate array of n+1 strings
        // Copy all strings

        length = countOccurrencesOfSubstring(SEPARATOR, input) + 1;
        array = malloc(length * sizeof(STRING));

        STRING input_ptr = input;
        for (size_t i = 0; i < length; i++) {
            // get end of current substring
            STRING end_of_current_substring = input_ptr;
            while (strcmp(end_of_current_substring, SEPARATOR) != 0 && *end_of_current_substring != '\0') {
                ++end_of_current_substring;
            }

            size_t bytes_to_allocate = end_of_current_substring - input_ptr + 1;
            array[i] = malloc(bytes_to_allocate);
            strncpy(array[i], input_ptr, bytes_to_allocate - 1);
            array[i][bytes_to_allocate - 1] = '\0';
            input_ptr = end_of_current_substring;

            if (i != input_length - 1) { // skip separator if so
                input_ptr += LEN_SEPARATOR;
            }
        }
    }

    output->length = length;
    output->array = (STRING *) array;
    return output;
}

MultiStrings* copyMultiStrings(const MultiStrings* input) {
    size_t length = input->length;
    WRITEABLE_STRING* array = malloc(sizeof(STRING) * length);

    for (size_t i = 0; i < length; ++i) {
        size_t str_len = strlen(input->array[i]);
        array[i] = malloc(str_len + 1);
        strcpy(array[i], input->array[i]);
    }

    MultiStrings* output = malloc(sizeof(MultiStrings));
    *output = (MultiStrings) {
            .length = length,
            .array = malloc(sizeof(STRING) * length)
    };

    return output;
}

void freeMultiStrings(MultiStrings* ms) {
    for (size_t i = 0; i < ms->length; i++) {
        free((void *) ms->array[i]);
    }
    free(ms->array);
    free(ms);
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

STRING makeStringFromMultiStrings(MultiStrings *input) {
    // get total size of string = size of all strings + (n-1) * separator + 1
    // allocate
    // copy all strings up to (n-1) followed by separator
    // copy last string without separator

    const size_t ms_len = input->length;

    size_t total_length = ((ms_len - 1) * strlen(SEPARATOR)) + 1;
    for (size_t i = 0; i < ms_len; i++) {
        total_length += strlen(input->array[i]);
    }

    WRITEABLE_STRING output = malloc(total_length);
    WRITEABLE_STRING output_ptr = output;

    for (size_t i = 0; i < ms_len; i++) {
        strcpy(output_ptr, input->array[i]);
        output_ptr += strlen(input->array[i]);

        if (i != ms_len - 1) {
            strcpy(output_ptr, SEPARATOR);
            output_ptr += LEN_SEPARATOR;
        }
    }
    output_ptr[0] = '\0';

    return (STRING) output;
}

void freeStringFromMultiStrings(STRING str) {
    free((void*)str);
}
