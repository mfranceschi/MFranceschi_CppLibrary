//
// Created by Martin on 14/02/2020.
//

#include "Controller.h"
#include "Interface_Texts.h"
#include "VerbsContainer.h"
#include "View.h"

void controller_handle_exercise() {
    // TODO

    // Set title: exercises (it won't change)
    // Clear contents
    // Show table headers
    // Show one random verb

    list_t results_as_list = NULL;
    list_node* current_results;

    view_set_title(exercises_title, true);
    view_clear_contents();
    view_show_table_headers();

    results_as_list = container_getRandomVerb();
    current_results = list_head(results_as_list);
    view_show_verbs_list(current_results);
    view_refresh_screen();

    view_get_search_command();
}
