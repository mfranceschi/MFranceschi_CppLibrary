//
// Created by Martin on 14/02/2020.
//

#ifndef IRREGULARVERBS_INCLUDECURSES_H
#define IRREGULARVERBS_INCLUDECURSES_H

/**
 * This module is only made to be included anywhere you need the Curses API.
 * It makes the right include directive for NCurses or PDCurses.
 */

/* copy of the CMakeLists, to be removed later
if(${CURSES_HAVE_CURSES_H})
    set(CURSES_INCLUDE_DIRECTORY curses.h)
    elseif(${CURSES_HAVE_NCURSES_H})
    set(CURSES_INCLUDE_DIRECTORY ncurses.h)
    elseif(${CURSES_HAVE_NCURSES_NCURSES_H})
    set(CURSES_INCLUDE_DIRECTORY ncurses/ncurses.h)
    elseif(CURSES_HAVE_NCURSES_CURSES_H)
    set(CURSES_INCLUDE_DIRECTORY ncurses/curses.h)
endif()
*/

#if 1 // CURSES_INCLUDE_DIRECTORY == curses.h // TODO why does it not works
#include <ncurses/ncurses.h>
#endif

#endif //IRREGULARVERBS_INCLUDECURSES_H
