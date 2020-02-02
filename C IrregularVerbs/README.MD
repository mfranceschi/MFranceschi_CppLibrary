# Irregular Verbs

## Summary

This simple program shall allow the user to visualize and test (or even train) its knowledge with irregular verbs in some language (originally German).

The verbs are dealed with like they are tuples of (infinitive form, translation, form at time 1, form at time 2), with every field being easily customizable. Even more: each field can hold several values for each verb (example: several translations for one verb).

At the beginning, it reads a CSV file and stores all the verbs in memory. Then the user is given a graphical interface that allows him/her to choose its next activity between:

* List verbs in alphabetical order and filtered by first letter.
* Search any verb by a substring.
* Test its knowledge by doing some exercise. One line is displayed, an incomplete line that the user shall fill the blanks in. The user can also ask for the solution.

## Implementation proposition

* Language: `C`
* Display: CLI with `curses`
* Verbs in-memory storage: SQLite
* Design patterns: MVC, ?
* No threads
* A Python script fetches the verbs from `https://www.allemandfacile.com/exercices/exercice-allemand-2/exercice-allemand-22677.php` (only once, if the `.CSV` file is missing).

## Windows concepts

### Welcome screen
```
####################################
#          (some title)            #
####################################


          Now loading...


```

### Main menu screen
```
####################################
#          (some title)            #
####################################

(Some guideline)...

1 - (choice 1, list)
2 - (choice 2, search)
3 - (choice 3, exercise)
4 - (choice 4, quit)
```

### List screen

Each time the user presses an alphabetic key, the screen updates with the new results. 
The user may quit this screen using the ESCAPE key.

```
##################################################
#                      List - k                  # (For letter k)
##################################################

(infinitive) | (translation) | (time 1) | (time 2) (Headers)
==================================================
(infinitive) | (translation) | (time 1) | (time 2) (Instance 1)
(infinitive) | (translation) | (time 1) | (time 2) (Instance 2)
...
```

### Search screen

Each time the user presses an alphabetic key or BACKSPACE, the screen updates with the new results. An empty input (after several BACKSPACES) provides no results. The user may quit using the ESCAPE key. 

```
##################################################
#                  Search - xyz                  # (For input xyz)
##################################################

(infinitive) | (translation) | (time 1) | (time 2) (Headers)
==================================================
(infinitive) | (translation) | (time 1) | (time 2) (Instance 1)
(infinitive) | (translation) | (time 1) | (time 2) (Instance 2)
...
```
