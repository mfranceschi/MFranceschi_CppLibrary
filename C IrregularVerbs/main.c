//
// Created by mfran on 15/01/2020.
//

#include <ncurses/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include "SQLite/sqlite3.h"
#include "Utils.h"
#include "Verb.h"
#include "VerbsContainer.h"
#include "View.h"

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
    makeVerbFromStrings("\"infinitif\"", "\"translation\"", "\"time1\"", "\"time2\"", v);
    printf("%i", container_addVerbs((const Verb **) &v, 1));
    freeVerb(v);
}

int main(int nargs, char** args) {
    fprintf(stdout, "It works!\n");

    container_start_up();
    tests_make_verb();
    fillVerbsContainer();
    container_shut_down();
    //system("pause");
    return EXIT_SUCCESS;
}
