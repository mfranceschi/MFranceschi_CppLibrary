//
// Created by Martin on 16/02/2020.
//

#ifndef IRREGULARVERBS_NCURSES_OUTPUT_DRIVER_H
#define IRREGULARVERBS_NCURSES_OUTPUT_DRIVER_H

#include "NCurses_Commons.h"
#include "Strings.h"
#include "../Verb.h"

/**
 * Sets the current view's Title.
 *
 * @param t The new title.
 * @param centered If true then title is centered, else it is left-aligned.
 */
void ncurses_output_title(STRING new_title, bool centered);

/**
 * Displays the table headers at top of the contents window.
 * Then the cursor is located below the header.
 */
void ncurses_output_table_headers();

/**
 * Prints the given text at center of the contents window.
 *
 * @param s Text to be printed.
 */
void ncurses_output_text_at_center_of_contents(STRING s);

void ncurses_output_verbs_list(void* verbs);

void ncurses_output_main_menu();

#endif //IRREGULARVERBS_NCURSES_OUTPUT_DRIVER_H
