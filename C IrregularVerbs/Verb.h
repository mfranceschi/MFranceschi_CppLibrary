//
// Created by mfran on 15/01/2020.
//

#ifndef IRREGULARVERBS_VERB_H
#define IRREGULARVERBS_VERB_H

#include <stdbool.h>

typedef const char* STRING;

typedef struct MultiStrings_s {
    size_t length;
    STRING* array;
} MultiStrings;

typedef struct Verb_s {
    MultiStrings infinitive;
    MultiStrings translation;
    MultiStrings time1;
    MultiStrings time2;
} Verb;

bool matchesInfinitive(STRING, const Verb*);
bool matchesTranslation(STRING, const Verb*);
bool matchesTime1(STRING, const Verb*);
bool matchesTime2(STRING, const Verb*);

void makeMultiStrings(STRING input, MultiStrings* output);
void freeMultiStrings(MultiStrings*);

void makeVerbFromStrings(STRING infinitive, STRING translation, STRING time1, STRING time2, Verb* output);
void makeVerbFromMultiStrings(MultiStrings* infinitive, MultiStrings* translation, MultiStrings* time1, MultiStrings* time2, Verb* output);
void freeVerb(Verb*);

#endif //IRREGULARVERBS_VERB_H
