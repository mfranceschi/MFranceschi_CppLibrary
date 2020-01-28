//
// Created by mfran on 24/01/2020.
//

#include "Interface_Texts.h"
#if LANG_INTERFACE == French

/* WELCOME SCREEN */
const STRING welcome_screen_title = "Verbes irréguliers";
const STRING welcome_screen_central_text = "Merci de patienter...";

/* MAIN MENU */
const STRING main_menu_title = "Menu principal";
const STRING main_menu_header = "Tapez la lettre correspondante à votre choix";
const STRING main_menu_choices_verbose[4] = {
        "Liste alphabétique des verbes",
        "Recherche manuelle",
        "Exercices",
        "Quitter"
};

/* LIST VERBS BY FIRST LETTER */
const STRING list_title_beginning = "Liste";
const STRING list_column_headers[4] = {
        "Infinitif", "Traduction",
#if LANG_VERBS == German
        "Prétérit", "Parfait"
#endif
};

/* MISCELLANEOUS */
const STRING dummy_welcome = "Message simplet de bienvenue !";

#endif
