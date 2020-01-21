//
// Created by mfran on 21/01/2020.
//

#include "FileStreamManipulations.h"
#include <stdio.h>
#include <unistd.h>

bool file_can_be_read(STRING filename) {
    return !access(filename, R_OK);
}

Verb* get_verb_from_csv_line(FILE* line) {
    size_t BUFFERS_SIZE = 200;
    WRITEABLE_STRING buffer_infinitive =  calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_translation = calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_time1 =       calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_time2 =       calloc(BUFFERS_SIZE, 1);
    Verb *result;

    fscanf(line, R"format("%s","%s","%s","%s\n")format", buffer_infinitive, buffer_translation, buffer_time1, buffer_time2);
    if (*buffer_infinitive && *buffer_translation && *buffer_time1 && *buffer_time2) {
        result = makeVerbFromStrings(buffer_infinitive, buffer_translation, buffer_time1, buffer_time2);
    } else {
        result = NULL;
    }
    free(buffer_infinitive);
    free(buffer_translation);
    free(buffer_time1);
    free(buffer_time2);
    return result;
}

void skip_line(FILE* stream) {
    while(getc(stream) != '\n');
}
