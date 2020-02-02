//
// Created by mfran on 01/02/2020.
//

#ifndef IRREGULARVERBS_NCURSES_INPUTS_DRIVER_H
#define IRREGULARVERBS_NCURSES_INPUTS_DRIVER_H

#include <curses.h>
#include "Utils.h"
#include "../View.h"

Command ncurses_input_user_choice(WINDOW* window, bool can_go_back);
CHARACTER ncurses_input_user_letter(WINDOW* window, int row_i, int col_i, bool can_escape);
void ncurses_input_start_up();
void ncurses_input_shut_down();

#endif //IRREGULARVERBS_NCURSES_INPUTS_DRIVER_H
