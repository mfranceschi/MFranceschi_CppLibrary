//
// Created by mfran on 24/01/2020.
//

#include <ctype.h>
#include <stdio.h>
#include "Texts/Interface_Texts.h"
#include "utils/Utils.h"
#include "VerbsContainer.h"
#include "View.h"

#define DEFAULT_LIST_LETTER 's'

static void _controller_start_up(va_list list) {
    UNUSED(list);
    view_start_up();
    view_show_welcome_screen(welcome_screen_title, welcome_screen_central_text);
    container_start_up();
    fill_verbs_container();
}

static void _controller_shut_down(va_list list) {
    UNUSED(list);
    container_shut_down();
    view_shut_down();
}

void controller_handle_list() {
    size_t len_of_title = strlen(list_title_beginning) + strlen(" - ") + 1;
    WRITEABLE_STRING title = calloc(len_of_title + 1, 1);
    char input = DEFAULT_LIST_LETTER;
    list_t results_as_list = NULL;
    list_node* current_results;
    int offset = 0;
    view_show_table_headers();

    while(input != KB_KEY_ESC) {
        switch (input) {
            case KB_KEY_UP:
                // just go one line higher
                if (offset) {
                    offset--;
                }
                break;
            case KB_KEY_DOWN:
                // just go one line lower
                if (offset < list_length(results_as_list) - 1) {
                    offset++;
                }
                break;
            default:
                if (isalpha(input)) { // i cannot find why it is necessary (^D sometimes when lateral arrow keys)
                    snprintf(title, len_of_title + 1, "%s - %c", list_title_beginning, input);
                    view_set_title(title, true);

                    results_as_list = container_getVerbsByFirstLetter(input);
                    offset = 0;
                }
                break;
        }
        current_results = list_head(results_as_list);
        for (int i=0; i<offset; i++) {
            current_results = current_results->next;
        }
        view_show_verbs_list(current_results);
        view_refresh_screen();
        input = view_ask_user_letter(true, true, false);
    }
    free(title);
}

void controller_handle_search() {
    size_t len_of_title = strlen(list_title_beginning) + strlen(" - ") + 1;
    WRITEABLE_STRING title = calloc(len_of_title + 1, 1);
    char input = DEFAULT_LIST_LETTER;
    list_t results_as_list = NULL;
    list_node* current_results;
    int offset = 0;
    view_show_table_headers();

    results_as_list = container_getVerbsBySubstring("berg");
    current_results = list_head(results_as_list);
    view_show_table_headers();
    view_show_verbs_list(current_results);
    view_refresh_screen();
    view_ask_user_choice(true); // TODO same

    free(title);
}

int run() {
    Command input_command = BACK_HOME;
    list_t current_results;
    run_and_wait(500, _controller_start_up);

    while (input_command != QUIT) {
        view_show_main_menu(main_menu_title, main_menu_header, (STRING *) &main_menu_choices_verbose);
        input_command = view_ask_user_choice(false);
        switch (input_command) {
            case LIST:
                view_clear_contents();
                controller_handle_list();
                break;

            case SEARCH:
                view_clear_contents();
                controller_handle_search();
                break;

            case EXERCISE: // TODO one day maybe
            default:
                break; // TODO
        }
        container_freeResults();
    }
    _controller_shut_down(NULL);
    return EXIT_SUCCESS;
}
