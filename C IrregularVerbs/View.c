//
// Created by Martin on 17/01/2020.
//

#include "Texts/Interface_Texts.h"
#include <locale.h>
#include "View.h"
#include "Verb.h"
#include "utils/NCurses_Inputs_Driver.h" // it includes curses
#include "utils/Utils.h"

struct x{struct x* next; Verb* verb;};
#define ROW_OF_HEADER 1
#define len_cols ((getmaxx(contents_win) / 4) - 2)

static WINDOW* title_win = NULL;
static WINDOW* title_text_win = NULL;
static WINDOW* contents_win = NULL;

#define COLOR_DEFAULT 1
#define COLOR_PAIR_ONE 2
#define COLOR_PAIR_TWO 3
#define COLOR_PAIR_THREE 4
#define COLOR_VERB_LOOP(cur) (\
    (cur == COLOR_PAIR_ONE) ? COLOR_PAIR_TWO : \
    (cur == COLOR_PAIR_TWO) ? COLOR_PAIR_THREE : \
    COLOR_PAIR_ONE)

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

void view_set_title(STRING new_title, bool centered) {
    if (centered) {
        _print_at_center(title_text_win, 0, 0, getmaxx(title_text_win), new_title);
    } else {
        wclear(title_text_win);
        mvwaddstr(title_text_win, 0, 0, new_title);
    }
}

void view_start_up() {
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

void view_show_welcome_screen(STRING title, STRING central_text) {
    noecho();
    view_set_title(title, true);
    wclear(contents_win);
    _print_at_center(title_win, 1, 0, getmaxx(title_win), central_text);
    view_refresh_screen();
}

void view_show_main_menu(STRING title, STRING guideline, STRING *choices) {
    view_set_title(title, true);
    wclear(contents_win);
    mvwaddstr(contents_win, 0, 0, guideline);
    mvwprintw(contents_win, 2, 0, "[L] - %s. \n[S] - %s. \n[E] - %s. \n[Q] - %s. ", choices[0], choices[1], choices[2], choices[3]);
    view_refresh_screen();
}

Command view_ask_user_choice(bool can_go_back) {
    return ncurses_input_user_choice(contents_win, can_go_back);
}

CHARACTER view_ask_user_letter(bool can_escape, bool can_arrows, bool can_backspace) {
    wmove(title_text_win, 0, 0);
    return ncurses_input_user_letter(title_text_win, can_escape, can_arrows, can_backspace);
}

void view_show_verbs_list(void *verbs) {
    struct x * pointed_struct;
    int current_row = ROW_OF_HEADER + 2;
    int current_color_index = COLOR_PAIR_ONE;
    while (verbs != NULL && current_row < getmaxy(contents_win)) {
        pointed_struct = verbs;

        wattron(contents_win, COLOR_PAIR(current_color_index));
        current_row = _print_one_verb(contents_win, current_row, 0, len_cols, false, getmaxx(contents_win) - current_row, pointed_struct->verb);
        wattroff(contents_win, COLOR_PAIR(current_color_index));

        current_color_index = COLOR_VERB_LOOP(current_color_index);
        verbs = pointed_struct->next;
        ++current_row;
    }
    wmove(contents_win, current_row, 0);
    wclrtobot(contents_win);
}

void view_show_table_headers() {
    _print_one_row(contents_win, ROW_OF_HEADER, 0, len_cols, true, verb_column_headers);
    wmove(contents_win, ROW_OF_HEADER + 1, 0);
    for(int i=0; i<getmaxx(contents_win); i++) {
        waddch(contents_win, '=');
    }
}

STRING view_get_search_string() {
    ncurses_input_buffer_reset();
    wmove(title_text_win, 0, 0);
    view_set_title(search_title_beginning, false);
    waddstr(title_text_win, " - ");
    ncurses_input_buffer_handle_user_input(title_text_win);
    return ncurses_input_buffer_get();
}

Buffer_Command view_get_search_command() {
    wmove(title_text_win, 0, 0);
    return ncurses_input_buffer_get_command(title_text_win);
}

void view_refresh_screen() {
    touchwin(title_win);
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

#undef ROW_OF_HEADER
