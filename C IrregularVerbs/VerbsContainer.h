//
// Created by mfran on 15/01/2020.
//

#ifndef IRREGULARVERBS_VERBSCONTAINER_H
#define IRREGULARVERBS_VERBSCONTAINER_H

#include <stdbool.h>
#include "Verb.h"

bool addVerb(const Verb*);
const Verb** getVerbsByInfinitive(STRING);
const Verb** getVerbsByTranslation(STRING);
const Verb** getVerbsByTime1(STRING);
const Verb** getVerbsByTime2(STRING);
void startUp();
void shutDown();

#endif //IRREGULARVERBS_VERBSCONTAINER_H
