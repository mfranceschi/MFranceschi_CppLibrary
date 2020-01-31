//
// Created by mfran on 24/01/2020.
//

#include "Texts/Interface_Texts.h"
#include "utils/Utils.h"
#include "VerbsContainer.h"
#include "View.h"

#define DEFAULT_LIST_LETTER 'b'

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

int run() {
    Command input_command = BACK_HOME;
    WRITEABLE_STRING list_letter_as_string = calloc(2, 1);
    *list_letter_as_string = DEFAULT_LIST_LETTER;
    list_t current_results;
    run_and_wait(500, _controller_start_up);

    while (input_command != QUIT) {
        view_show_main_menu(main_menu_title, main_menu_header, (STRING *) &main_menu_choices_verbose);
        input_command = view_ask_user_choice(false);
        switch (input_command) {
            case LIST:
                do {
                    current_results = container_getVerbsByFirstLetter(list_letter_as_string);
                    view_clear_contents();
                    view_show_verbs_list(list_title_beginning, verb_column_headers, list_head(current_results),
                            list_letter_as_string);
                } while((*list_letter_as_string = view_ask_user_letter(true)) != ESC);
                *list_letter_as_string = DEFAULT_LIST_LETTER;
                break;

            case SEARCH:
                current_results = container_getVerbsBySubstring("berg");
                view_show_verbs_list(search_title_beginning, verb_column_headers, list_head(current_results), "berg");
                view_ask_user_choice(true); // TODO same
                break;

            case EXERCISE: // TODO one day maybe
            default:
                break; // TODO
        }
        container_freeResults();
    }
    free(list_letter_as_string);
    _controller_shut_down();
    return EXIT_SUCCESS;
}