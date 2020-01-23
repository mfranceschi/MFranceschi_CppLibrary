//
// Created by mfran on 15/01/2020.
//

#include <ncurses/ncurses.h>
#include <stdio.h>
#include "Utils.h"
#include "Verb.h"
#include "VerbsContainer.h"
#include "View.h"

//#define printf(...) (void)(0) /* use this to forbid stdout display */

static void test_curses_works() {
    initscr();
    addstr("Curses works!");
    getch();
    endwin();
}

static void tests_make_verb() {
    Verb* v = makeVerbFromStrings("infinitive", "translation", "time1", "time2");
    printf("Success de l'ajout: %i\n", container_addVerbs((const Verb **) &v, 1));
    freeVerb(v);
    printf("Count: %u\n", container_getCount());

    list_t list = container_getVerbsBySubstring("ti");
    list_node* node = (list_node *) list_head(list);
    printf("Result length: %d\n", list_length(list));
    while (node != NULL) {
        printf("Verbe infinitif: '%s'\n", node->verb->infinitive->array[0]);
        node = list_item_next(node);
    }
    printf("Last error: '%s'\n", container_get_last_error());
    container_freeResults();
}

int main(int nargs, char** args) {
    printf("It works!\n");

    container_start_up();
    tests_make_verb();
    fillVerbsContainer();
    printf("DB size: %u\n", container_getCount());
    container_shut_down();
    system("pause");
    return EXIT_SUCCESS;
}
