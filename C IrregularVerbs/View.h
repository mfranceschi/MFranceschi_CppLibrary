//
// Created by mfran on 17/01/2020.
//

#ifndef IRREGULARVERBS_VIEW_H
#define IRREGULARVERBS_VIEW_H

#include "Strings.h"

typedef enum Commands_e {
    LIST, SEARCH, EXERCISE
} Commands;

void view_start_up();
void show_welcome_screen();
void show_main_menu();
void show_list_from_letter(char);
void update_list_from_letter(char, unsigned int nb_rows_scrolled_down);
void show_research_results(STRING);
void update_research_results(STRING, unsigned int nb_rows_scrolled_down);
void refresh_screen();
void clear_screen();
void view_shut_down();

#endif //IRREGULARVERBS_VIEW_H
