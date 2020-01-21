//
// Created by mfran on 21/01/2020.
//

#ifndef IRREGULARVERBS_FILESTREAMMANIPULATIONS_H
#define IRREGULARVERBS_FILESTREAMMANIPULATIONS_H

#include <stdio.h>
#include "../Strings.h"
#include "../Verb.h"

/**
 * @param filename A valid filename.
 * @return True if the file exists and can be read, false otherwise.
 */
bool file_can_be_read(STRING filename);

/**
 * Allocates a new verb from this line from the CSV.
 *
 * @param line A FILE* instance on which one valid line can be read.
 * @return The allocated verb, or NULL.
 */
Verb* get_verb_from_csv_line(FILE* line);

/**
 * @param stream Stream on which one line will be skipped.
 */
void skip_line(FILE* stream);

#endif //IRREGULARVERBS_FILESTREAMMANIPULATIONS_H
