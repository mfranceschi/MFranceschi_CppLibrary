//
// Created by mfran on 17/01/2020.
//

#include <curses.h>
#include "Texts/Interface_Texts.h"
#include <locale.h>
#include "View.h"
#include "Verb.h"
#include "utils/Utils.h"

struct x{struct x* next; Verb* verb;};

static WINDOW* title_win = NULL;
static WINDOW* title_text_win = NULL;
static WINDOW* contents_win = NULL;
static int title_win_max_x;
static int contents_win_max_x, contents_win_max_y;

/**
 * Prints some text at the center of the given space.
 * The unused space is filled with whitespaces.
 * No-op is string is too long. // TODO change
 *
 * @param win The window to print text in.
 * @param start_y Row of start in window.
 * @param start_x Column of start in window.
 * @param width Number of columns that we have.
 * @param text The text to print.
 */
static void _print_at_center(WINDOW* win, int start_y, int start_x, int width, STRING text);

/**
 * Prints one row similarly to printf("%s | %s | %s | %s\n", texts[0], ...).
 *
 * @param win The window in which we will print.
 * @param row_i The index of the row.
 * @param col_i The index of the column [NOT ACTUALLY USED, WE CONSIDER THE WHOLE LINE].
 * @param col_width The width of each individual column (text space allowed).
 * @param centered If true then we print at the center of each column.
 * @param texts The four strings to print.
 */
static void _print_one_row(WINDOW* win, int row_i, int col_i, int col_width, bool centered, const STRING texts[4]);

/**
 * Prints the given verb on one or several rows.
 *
 * @param win The window in which we will print.
 * @param row_i The index of the row.
 * @param col_i The index of the column [NOT ACTUALLY USED, WE CONSIDER THE WHOLE LINE].
 * @param col_width The width of each individual column (text space allowed).
 * @param centered If true then we print at the center of each column.
 * @param remaining The number or remaining allowed rows. We won't print more than remaining rows.
 * @param v The verb of course.
 * @return The current_row of the last printed row
 */
static int _print_one_verb(WINDOW* win, int row_i, int col_i, int col_width, bool centered, int remaining, Verb* v);

static void _print_one_row(WINDOW* win, int row_i, int col_i, int col_width, bool centered, const STRING texts[4]) {
    UNUSED(col_i);
    int cur_pos = 0;
    static const STRING separator = " | ";
    static const int sep_len = 3;
    int i;

    /* 1 - CLEAR THE LINE */
    wmove(win, row_i, 0);
    for (int j=0; j<getmaxx(win); j++) {
        waddch(win, ' ');
    }

    /* 2 - DO THE DAMN JOB */
    for (i=0; i<4; i++) {
        if (centered) {
            _print_at_center(win, row_i, cur_pos, col_width, texts[i]);
        } else {
            mvwaddstr(win, row_i, cur_pos, texts[i]);
        }
        wmove(win, row_i, cur_pos + col_width);
        if (i != 4-1) {
            waddstr(win, separator);
            cur_pos += col_width + sep_len;
        }
    }
    wmove(win, row_i + 1, 0);
}

static void _print_at_center(WINDOW* win, int start_y, int start_x, int width, STRING text) {
    int len = (int) strlen(text);
    if (len <= width) {
        mvwprintw(win, start_y, start_x, "%*s", width, "");
        mvwaddstr(win, start_y, start_x + ((width - len) / 2), text);
    }
}

static int _print_one_verb(WINDOW* win, int row_i, int col_i, int col_width, bool centered, int remaining, Verb* v) {
    MultiStrings decl[] = {
            *(v->infinitive),
            *(v->translation),
            *(v->time1),
            *(v->time2)
    };
    int max_iter = min_nbr(
            remaining,
            max_nbr_var(
                    4,
                    decl[0].length, decl[1].length, decl[2].length, decl[3].length));
    STRING texts[4];

    for (int i=0; i<max_iter; i++) {
        for(int j=0; j<4; j++) {
            texts[j] = (i < decl[j].length) ? decl[j].array[i] : "";
        }
        _print_one_row(win, row_i + i, col_i, col_width, centered, texts);
    }

    return row_i + max_iter - 1;
}

void view_set_title(STRING new_title) {
    _print_at_center(title_text_win, 0, 0, title_win_max_x, new_title);
}

void view_start_up() {
    setlocale(LC_ALL, ""); /* for wide characters support */
    initscr();
    keypad(stdscr, true); /* to be tested */
    cbreak();
    wclear(stdscr);

    /* initialize the windows */
    int screen_x, screen_y;
    getmaxyx(stdscr, screen_y, screen_x);
    title_win = newwin(3, 0, 0, 0);
    contents_win = newwin(screen_y - 3, 0, 3, 0);
    title_win_max_x = screen_x - 4;
    contents_win_max_x = screen_x;
    contents_win_max_y = screen_y - 3;
    title_text_win = derwin(title_win, 1, title_win_max_x, 1, 2);

    /* fill the title window */
    wmove(title_win, 0, 0);
    for (int i=0; i<screen_x; ++i) {
        waddch(title_win, '#');
    }
    mvwaddch(title_win, 1, 0, '#');
    mvwaddch(title_win, 1, screen_x - 1, '#');
    wmove(title_win, 2, 0);
    for (int i=0; i<screen_x; ++i) {
        waddch(title_win, '#');
    }
}

void view_show_welcome_screen(STRING title, STRING central_text) {
    noecho();
    view_set_title(title);
    wclear(contents_win);
    _print_at_center(title_win, 1, 0, contents_win_max_x, central_text);
    view_refresh_screen();
}

void view_show_main_menu(STRING title, STRING guideline, STRING *choices) {
    view_set_title(title);
    wclear(contents_win);
    mvwaddstr(contents_win, 0, 0, guideline);
    wmove(contents_win, 2, 0);
    wprintw(contents_win, "[L] - %s. \n[S] - %s. \n[E] - %s. \n[Q] - %s. ", choices[0], choices[1], choices[2], choices[3]);
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

void view_show_verbs_list(STRING title, STRING const *names, void *verbs, STRING title_precision) {
#define ROW_OF_HEADER 1
    view_clear_contents();

    /* 1 - PRINT TITLE */
    char* title_buffer = calloc(title_win_max_x, 1);
    snprintf(title_buffer, title_win_max_x, "%s - %s", title, title_precision);
    view_set_title(title_buffer);
    free(title_buffer);

    int len_cols = ((contents_win_max_x / 4) - 2);

    /* 2 - PRINT COLUMN HEADERS AND HORIZONTAL LINE */
    _print_one_row(contents_win, ROW_OF_HEADER, 0, len_cols, true, names);
    wmove(contents_win, ROW_OF_HEADER + 1, 0);
    for(int i=0; i<contents_win_max_x; i++) {
        waddch(contents_win, '=');
    }

    /* 3 - PRINT VERBS */
    struct x * pointed_struct;
    Verb* pointed_verb;
    STRING to_print[4];
    int current_row = ROW_OF_HEADER + 2;
    while (verbs != NULL && current_row < contents_win_max_y) {
        pointed_struct = verbs;
        pointed_verb = pointed_struct->verb;
        to_print[0] = makeStringFromMultiStrings(pointed_verb->infinitive);
        to_print[1] = makeStringFromMultiStrings(pointed_verb->translation);
        to_print[2] = makeStringFromMultiStrings(pointed_verb->time1);
        to_print[3] = makeStringFromMultiStrings(pointed_verb->time2);

        current_row = _print_one_verb(contents_win, current_row, 0, len_cols, false, contents_win_max_y - current_row, pointed_struct->verb);

        for(int i=0; i<4; i++) {
            freeStringFromMultiStrings(to_print[i]);
        }
        verbs = pointed_struct->next;
        ++current_row;
    }

    view_refresh_screen();
#undef ROW_OF_HEADER
}

void view_refresh_screen() {
    touchline(title_win, 1, 1); // inform title_win that its content are going to change because of sub window
    wrefresh(title_text_win);
    wrefresh(title_win);
    wrefresh(contents_win);
}

void view_clear_contents() {
    werase(contents_win);
}

void view_shut_down() {
    delwin(title_text_win);
    delwin(contents_win);
    delwin(title_win);
    endwin();
}
