//
// Created by mfran on 24/01/2020.
//

#include "Controller.h"
#include "Utils.h"
#include "VerbsContainer.h"
#include "View.h"

int run() {
    view_start_up();
    show_welcome_screen();
    container_start_up();
    fillVerbsContainer();
    show_main_menu();
    view_ask_user_choice(false);
    container_shut_down();
    view_shut_down();
    return EXIT_SUCCESS;
}