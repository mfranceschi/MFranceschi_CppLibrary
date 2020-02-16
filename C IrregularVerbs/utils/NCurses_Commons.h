//
// Created by Martin on 16/02/2020.
//

#ifndef IRREGULARVERBS_NCURSES_COMMONS_H
#define IRREGULARVERBS_NCURSES_COMMONS_H

#include "../Verb.h"

struct x{struct x* next; Verb* verb;}; // Dummy struct representing a Verb in a linked list.
static const int ROW_OF_HEADER = 1;

#define COLOR_DEFAULT 1
#define COLOR_PAIR_ONE 2
#define COLOR_PAIR_TWO 3
#define COLOR_PAIR_THREE 4

#define COLOR_VERB_LOOP(cur) cur = (\
    (cur == COLOR_PAIR_ONE) ? COLOR_PAIR_TWO : \
    (cur == COLOR_PAIR_TWO) ? COLOR_PAIR_THREE : \
    COLOR_PAIR_ONE)

/**
 * Starts, allocates and run every view initialization script.
 */
void ncurses_start_up();

/**
 * If relevant, performs a screen refresh.
 */
void ncurses_refresh_screen();

/**
 * Cleans the contents window but does not perform an explicit refresh.
 */
void ncurses_clear_contents();

/**
 * Closes and releases any resource associated with the view.
 */
void ncurses_shut_down();

void* ncurses_get_title_text_window();
void* ncurses_get_contents_window();
int ncurses_get_len_cols();
int ncurses_get_current_color_index();

#endif //IRREGULARVERBS_NCURSES_COMMONS_H
