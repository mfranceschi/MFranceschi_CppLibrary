//
// Created by mfran on 24/01/2020.
//

#include "Texts/Interface_Texts.h"
#include "utils/Utils.h"
#include "VerbsContainer.h"
#include "View.h"

static void _controller_start_up(va_list list) {
    UNUSED(list);
    view_start_up();
    view_show_welcome_screen(welcome_screen_title, welcome_screen_central_text);
    container_start_up();
    fillVerbsContainer();
}

static void _controller_shut_down() {
    container_shut_down();
    view_shut_down();
}

int run() {
    Command input = BACK_HOME;
    list_t current_results;
    run_and_wait(500, _controller_start_up);

    while (input != QUIT) {
        view_show_main_menu(main_menu_title, main_menu_header, (STRING *) &main_menu_choices_verbose);
        input = view_ask_user_choice(false);
        switch (input) {
            case LIST:
                current_results = container_getVerbsByFirstLetter("a");
                view_show_verbs_list(list_title_beginning, list_column_headers, list_head(current_results), "a");
                view_ask_user_choice(true); // TODO handle correctly
                break;
            default:
                break; // TODO
        }
        container_freeResults();
    }
    _controller_shut_down();
    return EXIT_SUCCESS;
}