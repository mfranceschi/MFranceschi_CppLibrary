//
// Created by mfran on 24/01/2020.
//

#include <ctype.h>
#include <stdio.h>
#include "Texts/Interface_Texts.h"
#include "utils/Utils.h"
#include "VerbsContainer.h"
#include "View.h"

#define DEFAULT_LIST_LETTER 'g'

static void _controller_start_up(va_list list) {
    UNUSED(list);
    view_start_up();
    view_show_welcome_screen(welcome_screen_title, welcome_screen_central_text);
    container_start_up();
    fill_verbs_container();
}

static void _controller_shut_down() {
    container_shut_down();
    view_shut_down();
}

void controller_handle_list() {
    size_t len_of_title = strlen(list_title_beginning) + strlen(" - ") + 1;
    WRITEABLE_STRING title = calloc(len_of_title + 1, 1);
    char input = DEFAULT_LIST_LETTER;
    list_node* current_results;
    int offset = 0;

    snprintf(title, len_of_title + 1, "%s - %c", list_title_beginning, input);
    view_set_title(title, true);
    view_show_table_headers(verb_column_headers);
    view_refresh_screen();

    while(input != KB_KEY_ESC) {
        switch (input) {
            case KB_KEY_UP:
                // just go one line higher
                break;
            case KB_KEY_DOWN:
                // just go one line lower
                break;
            default:
                // isalpha is true
                title[len_of_title - 1] = input;
                view_set_title(title, true);

                current_results = list_head(container_getVerbsByFirstLetter(input));
                offset = 0;

                for (int i=0; i<offset; i++) {
                    current_results = current_results->next;
                }
                view_show_verbs_list(current_results);
                break;
        }
        view_refresh_screen();
        input = view_ask_user_letter(true, true);
    }
    free(title);
}

int run() {
    Command input_command = BACK_HOME;
    list_t current_results;
    run_and_wait(500000, _controller_start_up);

    while (input_command != QUIT) {
        view_show_main_menu(main_menu_title, main_menu_header, (STRING *) &main_menu_choices_verbose);
        input_command = view_ask_user_choice(false);
        switch (input_command) {
            case LIST:
                view_clear_contents();
                controller_handle_list();
            case SEARCH:
                view_clear_contents();
                current_results = container_getVerbsBySubstring("berg");
                view_show_verbs_list(current_results);
                view_ask_user_choice(true); // TODO same
                break;

            case EXERCISE: // TODO one day maybe
            default:
                break; // TODO
        }
        container_freeResults();
    }
    _controller_shut_down();
    return EXIT_SUCCESS;
}
