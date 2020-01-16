//
// Created by Martin on 16/01/2020.
//

#include "Verb.h"

static bool _matchesField(STRING potential, MultiStrings* fieldFromVerb, bool strictly) {
    return strictly ?
        matchesMultiStrings(potential, fieldFromVerb) :
        isSubstringOfMultiStrings(potential, fieldFromVerb);
}

bool matchesInfinitive(STRING potential, const Verb* verb, bool strictly) {
    return _matchesField(potential, verb->infinitive, strictly);
}

bool matchesTranslation(STRING potential, const Verb* verb, bool strictly) {
    return _matchesField(potential, verb->translation, strictly);
}

bool matchesTime1(STRING potential, const Verb* verb, bool strictly) {
    return _matchesField(potential, verb->time1, strictly);
}

bool matchesTime2(STRING potential, const Verb* verb, bool strictly) {
    return _matchesField(potential, verb->time2, strictly);
}

void makeVerbFromStrings(STRING infinitive, STRING translation, STRING time1, STRING time2, Verb* output) {
    MultiStrings* inf = malloc(sizeof(MultiStrings));
    MultiStrings* tra = malloc(sizeof(MultiStrings));
    MultiStrings* ti1 = malloc(sizeof(MultiStrings));
    MultiStrings* ti2 = malloc(sizeof(MultiStrings));

    makeMultiStrings(infinitive, inf);
    makeMultiStrings(translation, tra);
    makeMultiStrings(time1, ti1);
    makeMultiStrings(time2, ti2);

    makeVerbFromMultiStrings(inf, tra, ti1, ti2, output);
}

void makeVerbFromMultiStrings(MultiStrings* infinitive, MultiStrings* translation, MultiStrings* time1, MultiStrings* time2, Verb* output) {
    output->infinitive = infinitive;
    output->translation = translation;
    output->time1 = time1;
    output->time2 = time2;
}

void freeVerb(Verb* v) {
    freeMultiStrings(v->infinitive);
    free(v->infinitive);

    freeMultiStrings(v->translation);
    free(v->translation);

    freeMultiStrings(v->time1);
    free(v->time1);

    freeMultiStrings(v->time2);
    free(v->time2);
}
