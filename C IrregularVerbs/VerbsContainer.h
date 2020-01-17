//
// Created by mfran on 15/01/2020.
//

#ifndef IRREGULARVERBS_VERBSCONTAINER_H
#define IRREGULARVERBS_VERBSCONTAINER_H

#include <stdbool.h>
#include "Verb.h"

typedef struct QueryResults_s {
    size_t length;
    Verb** results;
} QueryResults;

bool container_addVerb(const Verb*);
bool container_addVerbs(const Verb**, size_t number);
QueryResults* container_getVerbsByInfinitive(STRING);
QueryResults* container_getVerbsByTranslation(STRING);
QueryResults* container_getVerbsByTime1(STRING);
QueryResults* container_getVerbsByTime2(STRING);
void container_freeQueryResults(QueryResults*);
STRING container_get_last_error();
void container_start_up();
void container_shut_down();

#endif //IRREGULARVERBS_VERBSCONTAINER_H
