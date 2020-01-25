//
// Created by mfran on 17/01/2020.
//

#include <curses.h>
#include <string.h>
#include "Texts/Interface_French.h"
#include <unistd.h>
#include "utils/Utils.h"
#include "View.h"

void view_start_up() {
    initscr();
    keypad(stdscr, true); /* to be tested */
    cbreak();
}

void show_welcome_screen() {
    noecho();
    addstr(dummy_welcome); // TODO
    wmove(stdscr, 1, 0);
    refresh_screen();
}

void show_main_menu() {
    // TODO
}

Command view_ask_user_choice(bool can_go_back) {
#define _SELECTED_FORBIDDEN_VALUE -1
    int selected = _SELECTED_FORBIDDEN_VALUE;
    Command output = -1;
    while (selected == _SELECTED_FORBIDDEN_VALUE) {
        selected = wgetch(stdscr);

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
