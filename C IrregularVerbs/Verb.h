//
// Created by mfran on 15/01/2020.
//

#ifndef IRREGULARVERBS_VERB_H
#define IRREGULARVERBS_VERB_H

#include <stdbool.h>

typedef const char* STRING;

typedef struct Verb_s {

} Verb;

bool matchesInfinitive(STRING, const Verb*);
bool matchesTranslation(STRING, const Verb*);
bool matchesTime1(STRING, const Verb*);
bool matchesTime2(STRING, const Verb*);

#endif //IRREGULARVERBS_VERB_H
