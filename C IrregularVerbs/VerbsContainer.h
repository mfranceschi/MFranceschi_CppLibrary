//
// Created by mfran on 15/01/2020.
//

#ifndef IRREGULARVERBS_VERBSCONTAINER_H
#define IRREGULARVERBS_VERBSCONTAINER_H

#include "Contiki_2.6/list.h"
#include <stdbool.h>
#include "Verb.h"

typedef struct list_node_s {
    struct list_node_s * next;
    Verb* verb;
} list_node ;

bool container_addVerbs(const Verb**, size_t number);
list_t container_getVerbsBySubstring(STRING);
list_t container_getAllVerbs();
list_t container_getVerbsByFirstLetter(STRING);
size_t container_getCount();
void container_freeResults();
STRING container_get_last_error();
bool container_start_up();
void container_shut_down();
bool container_is_working();

#endif //IRREGULARVERBS_VERBSCONTAINER_H
