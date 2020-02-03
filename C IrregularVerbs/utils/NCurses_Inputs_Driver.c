//
// Created by Martin on 01/02/2020.
//

#include "NCurses_Inputs_Driver.h"
#include <ctype.h>

#define THE_BUFFER_LEN 50
CHARACTER the_buffer[THE_BUFFER_LEN] = {0};

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

CHARACTER ncurses_input_user_letter(WINDOW* window, bool can_escape, bool can_arrows, bool can_backspace) {
    int input;
    noecho(); cbreak(); keypad(window, true);

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

STRING ncurses_input_buffer_get() {
    return the_buffer;
}
void ncurses_input_buffer_reset() {
    memset(the_buffer, 0, 50 * sizeof(CHARACTER));
    the_buffer[0] = 's';
    the_buffer[1] = 'c';
    the_buffer[2] = 'h';
}

void ncurses_input_buffer_handle_user_input(WINDOW* window) {
    echo();
    wgetnstr(window, the_buffer, THE_BUFFER_LEN);
}

Buffer_Command ncurses_input_buffer_get_command(WINDOW* window) {
    int input;
    noecho();
    while (1) {
        input = wgetch(window);
        switch(input) {
            case KB_KEY_ESC:
                return BUFFER_BACK_HOME;

            case KEY_UP:
                return BUFFER_KEY_UP;

            case KEY_DOWN:
                return BUFFER_KEY_DOWN;

            case ' ':
                return BUFFER_RESET;

            default:
                break;
        }
    }
}
