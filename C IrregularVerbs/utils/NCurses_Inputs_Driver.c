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

CHARACTER ncurses_input_user_letter(WINDOW* window, int row_i, int col_i, bool can_escape, bool can_arrows) {
    int input;
    // ugly method but it is explicit
    while(1) {
        input = mvwgetch(window, row_i, col_i);
        if (can_escape && input == KB_KEY_ESC) {
            return KB_KEY_ESC;
        } else if (can_arrows) {
            if (input == KEY_UP || input == KEY_DOWN) {
                return input;
            }
        }
        else if (isalpha(input)) {
            return (CHARACTER)tolower(input);
        }
    }
}

