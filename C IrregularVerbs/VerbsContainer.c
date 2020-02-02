//
// Created by mfran on 16/01/2020.
//

#include <stdarg.h>
#include "VerbsContainer.h"
#include "utils/SQLiteDriver.h"

/* Count of verbs, in a cache for optimization. */
static size_t cache_count_of_verbs = 0;

/* List of current results. */
LIST(current_results);

/* Indicates whether the container is running now. */
static bool working = false;

static void _addVerbs_inALoopOf_addVerb(va_list arguments) {
    const Verb** verbs = va_arg(arguments, const Verb**);
    size_t number = va_arg(arguments, size_t);
    for (size_t i = 0; i < number; i++) {
        if (!m_sqlite_addVerb(verbs[i])) {
            return;
        }
    }
}

bool container_addVerbs(const Verb** verbs, size_t number) {
    if (m_sqlite_run_in_exclusive_write_transaction( _addVerbs_inALoopOf_addVerb, verbs, number)) {
        cache_count_of_verbs = 0;
        return true;
    } else {
        return false;
    }
}

list_t container_getVerbsBySubstring(STRING str) {
    container_freeResults();
    list_init(current_results);
    m_sqlite_search_substring(str, current_results);
    return current_results;
}

list_t container_getVerbsByFirstLetter(CHARACTER character) {
    static CHARACTER str_to_put_in_query [2] = {'\0', '\0'};
    str_to_put_in_query[0] = character;
    container_freeResults();
    list_init(current_results);
    m_sqlite_get_by_first_letters_of_infinitive(str_to_put_in_query, current_results);
    return current_results;
}

list_t container_getAllVerbs() {
    container_freeResults();
    list_init(current_results);
    m_sqlite_get_all(current_results);
    return current_results;
}

size_t container_getCount() {
    if (!cache_count_of_verbs) {
        cache_count_of_verbs = m_sqlite_get_count();
    }
    return cache_count_of_verbs;
}

void container_freeResults() {
    list_node* current_node;
    while ((current_node = list_head(current_results)) != NULL) {
        freeVerb(current_node->verb);
        list_pop(current_results);
    }
}

bool container_start_up() {
    return working = m_sqlite_start_up();
}

void container_shut_down() {
    return (working = false, m_sqlite_shut_down());
}

STRING container_get_last_error() {
    return m_sqlite_get_last_error_message();
}

bool container_is_working() {
    return working;
}
