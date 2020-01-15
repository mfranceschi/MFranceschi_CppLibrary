//
// Created by mfran on 15/01/2020.
//

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include "SQLite/sqlite3.h"

int main(int nargs, char** args) {
    fprintf(stdin, "It works!\n");
    initscr();
    addstr("Curses works!");
    getch();
    endwin();
    sqlite3_open(NULL, NULL);
    return EXIT_SUCCESS;
}
