//
// Created by mfran on 17/01/2020.
//

#include <ncurses/ncurses.h>
#include "View.h"

void view_start_up() {
    initscr();
    keypad(stdscr, true); /* to be tested */
}

void show_welcome_screen();

void show_main_menu();

Command view_ask_user_choice();

void show_list_from_letter(char);
void update_list_from_letter(char, unsigned int nb_rows_scrolled_down);
void show_research_results(STRING);
void update_research_results(STRING, unsigned int nb_rows_scrolled_down);

void refresh_screen() {
    refresh();
}

void clear_screen() {
    clear(); /* or erase? */
}

void view_shut_down() {
    endwin();
}
