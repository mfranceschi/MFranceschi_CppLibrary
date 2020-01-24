//
// Created by mfran on 17/01/2020.
//

#ifndef IRREGULARVERBS_VIEW_H
#define IRREGULARVERBS_VIEW_H

#include "Strings.h"

typedef enum Command_e {
    LIST, SEARCH, EXERCISE, BACK_HOME, QUIT
} Command;

/**
 * Starts, allocates and run every view initialization script.
 */
void view_start_up();

/**
 * Shows up a welcome screen without any menu.
 * May indicate that loading is in progress.
 */
void show_welcome_screen();

/**
 * Shows the program's main menu. The user can do any of the following:
 * - List verbs by first letter.
 * - Search verbs by a substring query.
 * - Do an exercise.
 * - Quit.
 * Note: the display is updated but the user does not have control yet.
 */
void show_main_menu();

/**
 * Returns the user's input as a Command.
 *
 * @param can_go_back True if the "go back" option is allowed for the user.
 * @return a Command corresponding to the user's input.
 */
Command view_ask_user_choice(bool can_go_back);

void show_list_from_letter(char);
void update_list_from_letter(char, unsigned int nb_rows_scrolled_down);
void show_research_results(STRING);
void update_research_results(STRING, unsigned int nb_rows_scrolled_down);

/**
 * If relevant, performs a screen refresh.
 */
void refresh_screen();

/**
 * Cleans the screen (empty or so) but does not perform an explicit refresh.
 */
void clear_screen();

/**
 * Closes and releases any resource associated with the view.
 */
void view_shut_down();

#endif //IRREGULARVERBS_VIEW_H
