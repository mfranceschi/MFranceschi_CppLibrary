//
// Created by mfran on 15/01/2020.
//

#include <ncurses/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include "SQLite/sqlite3.h"
#include "Verb.h"
#include "VerbsContainer.h"

static void test_curses_works() {
    initscr();
    addstr("Curses works!");
    getch();
    endwin();
}

static void test_sqlite_works() {
    sqlite3_open(NULL, NULL);
}

static void tests_make_verb() {
    Verb* v = malloc(sizeof(Verb));
    makeVerbFromStrings("infinitif", "translation", "time1", "time2", v);
    fprintf(stdout, "%p\n", v->infinitive);
    freeVerb(v);
    free(v);
}

int main(int nargs, char** args) {
    fprintf(stdout, "It works!\n");

    startUp();
    shutDown();
    system("pause");
    return EXIT_SUCCESS;
}
