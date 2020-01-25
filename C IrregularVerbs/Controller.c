//
// Created by mfran on 24/01/2020.
//

#include "utils/Utils.h"
#include "VerbsContainer.h"
#include "View.h"

static void _controller_start_up(va_list list) {
    UNUSED(list);
    view_start_up();
    show_welcome_screen();
    container_start_up();
    fillVerbsContainer();
}

static void _controller_shut_down() {
    container_shut_down();
    view_shut_down();
}

int run() {
    run_and_wait(500, _controller_start_up);
    show_main_menu();
    view_ask_user_choice(false);
    _controller_shut_down();
    return EXIT_SUCCESS;
}