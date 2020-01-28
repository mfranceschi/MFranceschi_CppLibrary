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
 *
 * @param title Title of that screen.
 * @param central_text The central text indicating that we are loading now.
 */
void show_welcome_screen(STRING title, STRING central_text);

/**
 * Shows the program's main menu. The user does not have control yet.
 *
 * @param title Title of that screen.
 * @param guideline The guideline below the title.
 * @param choices Explanation of each of the four options.
 */
void show_main_menu(STRING title, STRING guideline, STRING choices[4]);

/**
 * Returns the user's input as a Command.
 *
 * @param can_go_back True if the "go back" option is allowed for the user.
 * @return a Command corresponding to the user's input.
 */
Command view_ask_user_choice(bool can_go_back);

/**
 * Sets the current view's Title.
 *
 * @param t The new title.
 */
void view_set_title(STRING t);

/**
 * Show a list of verbs.
 * The `verbs` pointer shall point to a linked list of {.next: void*, .verb: Verb*} (declared in that order).
 *
 * @param title Title of that screen.
 * @param names Titles of the four columns displaying the verbs: inf, tra, ti1, ti2.
 * @param verbs Linked list of verbs.
 */
void view_show_verbs_list(STRING title, STRING names[4], void* verbs);

/**
 * If relevant, performs a screen refresh.
 */
void view_refresh_screen();

/**
 * Cleans the screen (empty or so) but does not perform an explicit refresh.
 */
void view_clear_screen();

/**
 * Closes and releases any resource associated with the view.
 */
void view_shut_down();

#endif //IRREGULARVERBS_VIEW_H
