//
// Created by mfran on 24/01/2020.
//

#include "Controller.h"
#include <sys/time.h>
#include "Utils.h"
#include "VerbsContainer.h"
#include "View.h"

static void _controller_start_up() {
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
    struct timeval time_of_start_up;
    gettimeofday(&time_of_start_up, NULL); /* TODO */
    _controller_start_up();
    //while(time(NULL) < time_of_start_up + 1);
    show_main_menu();
    view_ask_user_choice(false);
    _controller_shut_down();
    return EXIT_SUCCESS;
}