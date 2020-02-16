//
// Created by Martin on 17/01/2020.
//

#include "Texts/Interface_Texts.h"
#include <locale.h>
#include <math.h>
#include "View.h"
#include "Verb.h"
#include "utils/IncludeCurses.h"
#include "utils/NCurses_Commons.h"
#include "utils/NCurses_Input_Driver.h" // it includes curses
#include "utils/NCurses_Output_Driver.h"

/*static WINDOW* title_win = NULL;
static WINDOW* title_text_win = NULL;
static WINDOW* contents_win = NULL;*/

#define contents_win (WINDOW*) (ncurses_get_contents_window())
#define title_text_win (WINDOW*) (ncurses_get_title_text_window())
#define len_cols ((getmaxx(contents_win) / 4) - 2)

#define COLOR_DEFAULT 1
#define COLOR_PAIR_ONE 2
#define COLOR_PAIR_TWO 3
#define COLOR_PAIR_THREE 4
#define COLOR_VERB_LOOP(cur) cur = (\
    (cur == COLOR_PAIR_ONE) ? COLOR_PAIR_TWO : \
    (cur == COLOR_PAIR_TWO) ? COLOR_PAIR_THREE : \
    COLOR_PAIR_ONE)

/**
 * Prints some text at the center of the given space.
 * The unused space is filled with whitespaces.
 * No-op if string is too long. // TODO change
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
    int max_iter = (int) fminl(
            remaining,
            fmaxl(decl[0].length, fmaxl(decl[1].length, fmaxl(decl[2].length, decl[3].length)))
    );
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
    ncurses_output_title(new_title, centered);
}

void view_start_up() {
    ncurses_start_up();
}

void view_show_welcome_screen() {
    view_set_title(welcome_screen_title, true);
    ncurses_output_text_at_center_of_contents(welcome_screen_central_text);
    view_refresh_screen();
}

void view_show_main_menu() {
    view_set_title(main_menu_title, true);
    ncurses_output_main_menu();
    view_refresh_screen();
}

Command view_ask_user_choice(bool can_go_back) {
    return ncurses_input_user_choice(can_go_back);
}

CHARACTER view_ask_user_letter(bool can_escape, bool can_arrows, bool can_backspace) {
    return ncurses_input_user_letter(can_escape, can_arrows, can_backspace);
}

void view_show_verbs_list(void *verbs) {
    ncurses_output_verbs_list(verbs);
}

void view_show_table_headers() {
    ncurses_output_table_headers();
}

STRING view_get_search_string() {
    ncurses_input_buffer_reset();
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
    ncurses_refresh_screen();
}

void view_clear_contents() {
    ncurses_clear_contents();
}

void view_shut_down() {
    ncurses_shut_down();
}
