//
// Created by Martin on 01/02/2020.
//

#include "NCurses_Inputs_Driver.h"
#include <ctype.h>

Command ncurses_input_user_choice(WINDOW* window, bool can_go_back) {
#define _SELECTED_FORBIDDEN_VALUE -1
    int selected = _SELECTED_FORBIDDEN_VALUE;
    Command output = -1;
    while (selected == _SELECTED_FORBIDDEN_VALUE) {
        selected = wgetch(window);

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

CHARACTER ncurses_input_user_letter(WINDOW* window, int row_i, int col_i, bool can_escape, bool can_arrows, bool can_backspace) {
    int input;
    noecho(); cbreak(); keypad(window, true);
    wmove(window, row_i, col_i);

    // ugly method but it is explicit
    while(1) {
        input = wgetch(window);
        switch (input) {
            case KB_KEY_ESC:
                if (can_escape) {
                    return KB_KEY_ESC;
                }
                break;

            case KEY_UP:
                if (can_arrows) {
                    return KB_KEY_UP;
                }
                break;

            case KEY_DOWN:
                if (can_arrows) {
                    return KB_KEY_DOWN;
                }
                break;

            case KEY_BACKSPACE:
                if (can_backspace) {
                    return KB_KEY_BACKSPACE;
                }
                break;

            default:
                if (isalpha(input)) {
                    return (CHARACTER)tolower(input);
                }
                break;
        }
    }
}

