//
// Created by Martin on 16/02/2020.
//

#include "NCurses_Output_Driver.h"
#include "../Texts/Interface_Texts.h"
#include "IncludeCurses.h"
#include <math.h>

/**
 * A struct that is used to pass parameters uniformly accross some functions.
 * By convention it shall be passed as a non-const pointer.
 */
typedef struct PrintOnWindowSettings_s {
    WINDOW* window; // The window on which we will print.
    union { // Row of start in window.
        int start_y;
        int row_i;
    };
    union { // Column of start in window.
        int start_x;
        int col_i;
    };
    int width;      // Number of columns that we have.
    bool centered;  // True if the text should be centered.
    int color;      // Color of the printed text.
} PrintOnWindowSettings;

#define _MAKE_PRINTONWINDOWSETTINGS (PrintOnWindowSettings) {\
    .window = NULL, \
    .start_y = 0, \
    .start_x = 0, \
    .width = 0, \
    .centered = false, \
    .color = 0}
#define _COPY_PRINTONWINDOWSETTINGS(src) (PrintOnWindowSettings) {\
    .window = (src).window, \
    .start_y = (src).start_y, \
    .start_x = (src).start_x, \
    .width = (src).width, \
    .centered = (src).centered, \
    .color = (src).color}

/**
 * Prints some text at the center of the given space.
 * The unused space is filled with whitespaces.
 * No-op if string is too long. // TODO change ?
 *
 * @param settings The settings. The "centered" and "color" fields are ignored.
 * @param text The text to print.
 */
void ncurses_print_at_center(PrintOnWindowSettings* settings, STRING text);

/**
 * Prints one row similarly to printf("%s | %s | %s | %s\n", texts[0], ...).
 *
 * @param settings The settings.
 * @param texts The four strings to print.
 */
void ncurses_print_one_row(PrintOnWindowSettings* settings, const STRING texts[4]);

/**
 * Prints the given verb on one or several rows.
 *
 * @param settings The settings.
 * @param remaining The number or remaining allowed rows. We won't print more than remaining rows.
 * @param v The verb to print.
 * @return The current_row of the last printed row.
 */
int ncurses_print_one_verb(PrintOnWindowSettings* settings, int remaining, const Verb* v);




void ncurses_print_at_center(PrintOnWindowSettings* settings, STRING text) {
    int len = (int) strlen(text);
    if (len <= settings->width) {
        mvwprintw(settings->window, settings->start_y, settings->start_x, "%*s", settings->width, "");
        mvwaddstr(settings->window, settings->start_y, settings->start_x + ((settings->width - len) / 2), text);
    }
}

void ncurses_print_one_row(PrintOnWindowSettings* settings, const STRING texts[4]) {
    int cur_pos = 0;
    static const STRING separator = " | ";
    static const int sep_len = 3;
    int i;

    WINDOW* win = settings->window;
    int row_i = settings->start_y;
    int col_width = settings->width;
    PrintOnWindowSettings settings_new;

    /* 1 - CLEAR THE LINE */
    wmove(win, row_i, 0);
    for (int j=0; j<getmaxx(win); j++) {
        waddch(win, ' ');
    }

    /* 2 - DO THE DAMN JOB */
    for (i=0; i<4; i++) {
        wattron(win, COLOR_PAIR(settings->color));
        if (settings->centered) {
            settings_new = (PrintOnWindowSettings) {
                .window = win,
                .row_i = row_i,
                .col_i = cur_pos,
                .width = col_width
            };
            ncurses_print_at_center(&settings_new, texts[i]);
        } else {
            mvwaddstr(win, row_i, cur_pos, texts[i]);
        }
        wattroff(win, COLOR_PAIR(settings->color));
        wmove(win, row_i, cur_pos + col_width);
        if (i != 4-1) {
            waddstr(win, separator);
            cur_pos += col_width + sep_len;
        }
    }
    wmove(win, row_i + 1, 0);
}

int ncurses_print_one_verb(PrintOnWindowSettings* settings, int remaining, const Verb* v) {
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
    PrintOnWindowSettings settings1;

    for (int i=0; i<max_iter; i++) {
        for(int j=0; j<4; j++) {
            texts[j] = (i < decl[j].length) ? decl[j].array[i] : "";
        }
        settings1 = _COPY_PRINTONWINDOWSETTINGS(*settings);
        settings1.row_i += i;
        ncurses_print_one_row(&settings1, texts);
    }

    return settings1.row_i + max_iter - 1;
}




















void ncurses_output_verbs_list(void* verbs) {
    WINDOW* contents_win = ncurses_get_contents_window();
    struct x * pointed_struct;
    PrintOnWindowSettings settings = {
            .window = contents_win,
            .row_i = ROW_OF_HEADER + 2,
            .color = COLOR_PAIR_ONE,
            .width = getmaxx(contents_win),
            .centered = false
    };

    const int max_y = getmaxy(contents_win);
    pointed_struct = verbs;
    while (verbs != NULL && settings.row_i < max_y) {

        //current_row = _print_one_verb(contents_win, current_row, 0, len_cols, false, getmaxx(contents_win) - current_row, pointed_struct->verb);
        ncurses_print_one_verb(&settings, getmaxx(contents_win) - settings.row_i, pointed_struct->verb);

        COLOR_VERB_LOOP(settings.color);
        pointed_struct = pointed_struct->next;
        settings.row_i ++;
    }
    wmove(contents_win, settings.row_i, 0);
    wclrtobot(contents_win);
}





void ncurses_output_title(STRING new_title, bool centered) {
    WINDOW* title_text_win = ncurses_get_title_text_window();
    if (centered) {
        PrintOnWindowSettings settings = _MAKE_PRINTONWINDOWSETTINGS;
        settings.width = getmaxx(title_text_win);
        ncurses_print_at_center(&settings, new_title);
    } else {
        wclear(title_text_win);
        mvwaddstr(title_text_win, 0, 0, new_title);
    }
}

void ncurses_output_table_headers() {
    WINDOW* contents_win = ncurses_get_contents_window();

    PrintOnWindowSettings settings = _MAKE_PRINTONWINDOWSETTINGS;
    settings.row_i = ROW_OF_HEADER;
    settings.width = ncurses_get_len_cols();
    settings.centered = true;
    ncurses_print_one_row(&settings, verb_column_headers);

    wmove(contents_win, ROW_OF_HEADER + 1, 0);
    for(int i=0; i<getmaxx(contents_win); i++) {
        waddch(contents_win, '=');
    }
}

void ncurses_output_text_at_center_of_contents(STRING s) {
    WINDOW* contents_win = ncurses_get_contents_window();
    int row_of_middle = getmaxy(contents_win) / 2;
    PrintOnWindowSettings settings = {
            .row_i = row_of_middle,
            .width = getmaxx(contents_win),
            .window = contents_win,
    };
    ncurses_print_at_center(&settings, s);
}

void ncurses_output_main_menu() {
    WINDOW* contents_win = ncurses_get_contents_window();

    wclear(contents_win);
    mvwaddstr(contents_win, 0, 0, main_menu_header);
    mvwprintw(
            contents_win, 2, 0,
            "[L] - %s. \n[S] - %s. \n[E] - %s. \n[Q] - %s. ",
            main_menu_choices_verbose[0],
            main_menu_choices_verbose[1],
            main_menu_choices_verbose[2],
            main_menu_choices_verbose[3]);
}
