//
// Created by mfran on 16/02/2020.
//

#include <locale.h>
#include "IncludeCurses.h"
#include "NCurses_Commons.h"
#include "Utils.h"

#define len_cols ((getmaxx(contents_win) / 4) - 2)
static WINDOW* title_win = NULL;
static WINDOW* title_text_win = NULL;
static WINDOW* contents_win = NULL;

void ncurses_start_up() {
    setlocale(LC_ALL, ""); /* for wide characters support (failed!) */
    if (initscr() == NULL) {
        exit(EXIT_BECAUSE_CURSES_ISSUE);
    }
    wclear(stdscr);

    /* initialize the colors */
    // I do not check has_colors neither can_change_colors because using color functions cannot be harmful
    start_color();
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_PAIR_ONE, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_PAIR_TWO, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_THREE, COLOR_RED, COLOR_BLACK);

    /* initialize the windows */
    int screen_x, screen_y;
    getmaxyx(stdscr, screen_y, screen_x);
    title_win = newwin(3, 0, 0, 0);
    contents_win = newwin(screen_y - 3, 0, 3, 0);
    title_text_win = derwin(title_win, 1, screen_x - 4, 1, 2);

    /* fill the title window */
    wborder(title_win, '#', '#', '#', '#', '#', '#', '#', '#');
}

void ncurses_refresh_screen() {
    touchwin(title_win);
    wrefresh(title_text_win);
    wrefresh(title_win);
    wrefresh(contents_win);
}

void ncurses_clear_contents() {
    werase(contents_win);
}

void ncurses_shut_down() {
    delwin(title_text_win);
    delwin(contents_win);
    delwin(title_win);
    endwin();
}

void* ncurses_get_title_text_window() {
    return title_text_win;
}
void* ncurses_get_contents_window() {
    return contents_win;
}

int ncurses_get_len_cols() {
    return len_cols;
}
