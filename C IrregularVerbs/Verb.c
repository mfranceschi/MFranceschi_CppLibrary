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
    MultiStrings* inf = makeMultiStrings(infinitive);
    MultiStrings* tra = makeMultiStrings(translation);
    MultiStrings* ti1 = makeMultiStrings(time1);
    MultiStrings* ti2 = makeMultiStrings(time2);

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
    freeMultiStrings(v->translation);
    freeMultiStrings(v->time1);
    freeMultiStrings(v->time2);
    free(v);
}
