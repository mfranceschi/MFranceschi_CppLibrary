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

bool addVerb(const Verb*);
QueryResults* getVerbsByInfinitive(STRING);
QueryResults* getVerbsByTranslation(STRING);
QueryResults* getVerbsByTime1(STRING);
QueryResults* getVerbsByTime2(STRING);
void freeQueryResults(QueryResults*);
void startUp();
void shutDown();

#endif //IRREGULARVERBS_VERBSCONTAINER_H
