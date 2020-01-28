//
// Created by mfran on 17/01/2020.
//

#include <curses.h>
#include "Texts/Interface_French.h"
#include <locale.h>
#include "View.h"
#include "Verb.h"
#include "VerbsContainer.h"

struct x{struct x* next; Verb* verb;};

static WINDOW* title_win = NULL;
static WINDOW* contents_win = NULL;
static int title_win_max_x;
static int contents_win_max_x, contents_win_max_y;

void _show_verbs(list_t results) {
    struct x* y = list_head(results);
    Verb* v;
    while (y != NULL) {
        v = y->verb;
        wprintw(contents_win, "Verbe '%s': '%s'. \n", v->infinitive->array[0], v->translation->array[0]);
        y = list_item_next(y);
    }
    view_refresh_screen();
}

void _set_title(STRING new_title) {
    /* clear line */
    wmove(title_win, 1, 2);
    for (int i=0; i<(title_win_max_x-2); i++) {
        waddch(title_win, ' ');
    }

    /* print in center */
    wmove(title_win, 1, 2);
    waddstr(title_win, new_title);
}

void view_start_up() {
    setlocale(LC_ALL, "UTF-8"); /* for wide characters support */
    initscr();
    keypad(stdscr, true); /* to be tested */
    cbreak();

    /* initialize the windows */
    int screen_x, screen_y;
    getmaxyx(stdscr, screen_x, screen_y);
    title_win = newwin(3, 0, 0, 0);
    contents_win = newwin(screen_y - 3, 0, 3, 0);
    title_win_max_x = screen_x - 4;
    contents_win_max_x = screen_x;
    contents_win_max_y = screen_y - 3;

    /* fill the title window */
    wmove(title_win, 0, 0);
    for (int i=0; i<screen_x; ++i) {
        waddch(title_win, '#');
    }
    wmove(title_win, 1, 0);
    waddch(title_win, '#');
    wmove(title_win, 1, screen_x - 1);
    waddch(title_win, '#');
    wmove(title_win, 2, 0);
    for (int i=0; i<screen_x; ++i) {
        waddch(title_win, '#');
    }
}

void show_welcome_screen(STRING title, STRING central_text) {
    noecho();
    _set_title(title);
    wclear(contents_win);
    wmove(contents_win, 1, 0);
    waddstr(contents_win, central_text); // TODO
    view_refresh_screen();
}

void show_main_menu(STRING title, STRING guideline, STRING choices[4]) {
    _set_title(title);
    wclear(contents_win);
    mvwaddstr(contents_win, 0, 0, guideline);
    wmove(contents_win, 2, 0);
    wprintw(contents_win, "[L] - %s. \n[S] - %s. \n[E] - %s. \n[Q] - %s. ", choices[0], choices[1], choices[2], choices[3]);
    /*list_t res = container_getVerbsByFirstLetter("b");
    _show_verbs(res);
    container_freeResults();*/
    // TODO
    view_refresh_screen();
}

Command view_ask_user_choice(bool can_go_back) {
#define _SELECTED_FORBIDDEN_VALUE -1
    int selected = _SELECTED_FORBIDDEN_VALUE;
    Command output = -1;
    while (selected == _SELECTED_FORBIDDEN_VALUE) {
        selected = wgetch(contents_win);

        switch (selected) {
            case 'L':
            case 'l':
                output = LIST;
                break;
            case 'S':
            case 's':
                output = SEARCH;
                break;
            case 'E':
            case 'e':
                output = EXERCISE;
                break;
            case 'B':
            case 'b':
                if (can_go_back) {
                    output = BACK_HOME;
                } else {
                    selected = _SELECTED_FORBIDDEN_VALUE;
                }
                break;
            case 'Q':
            case 'q':
                output = QUIT;
                break;
            default:
                selected = _SELECTED_FORBIDDEN_VALUE;
        }

    }
    return output;
#undef _SELECTED_FORBIDDEN_VALUE
}

void view_refresh_screen() {
    wrefresh(title_win);
    wrefresh(contents_win);
}

void view_clear_screen() {
    clear(); /* or erase? */
}

void view_shut_down() {
    delwin(contents_win);
    delwin(title_win);
    endwin();
}
