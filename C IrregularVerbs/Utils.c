//
// Created by mfran on 17/01/2020.
//

#include "Low-Level Implementations/FileStreamManipulations.h"
#include <stdio.h>
#include <string.h>
#include "Utils.h"
#include "VerbsContainer.h"
#include "Verb.h"

STRING CSV_FILENAME = "../rsc/verbs.csv";
#define PYTHON_SCRIPT_FILENAME "../rsc/get_verbs.py"

/* ***** STATIC FUNCTIONS DECLARATION ***** */

/* ***** STATIC FUNCTIONS DEFINITION ***** */

/* ***** PUBLIC FUNCTIONS DEFINITION ***** */

void fillVerbsContainer() {

    // open file
    FILE* csv_file = NULL;
    if ((csv_file = fopen(CSV_FILENAME, "r")) == NULL) {
        if (
                system("python " PYTHON_SCRIPT_FILENAME) || /* Failure of PYTHON script */
                (csv_file = fopen(CSV_FILENAME, "r")) == NULL /* File still cannot be read */
                ) {
            exit(EXIT_BECAUSE_FILE_FAILURE);
        }
    }

    // prepare buffers
    const static size_t LEN_OF_LIST = 500;
    Verb** verb_list = malloc(sizeof(Verb*) * LEN_OF_LIST);

    // skip first line
    skip_line(csv_file);

    // for each line, build a verb
    size_t current_index = 0;
    Verb* current_verb;
    while ((current_verb = get_verb_from_csv_line(csv_file)) != NULL) {
        verb_list[current_index++] = current_verb;
    }

    // close file
    fclose(csv_file);

    // add verbs to container
    container_addVerbs((const Verb **) verb_list, current_index);

    // free verbs
    for (size_t i = 0; i < current_index; i++) {
        freeVerb(verb_list[i]);
    }
    free(verb_list);
}

/* Code snippet adapted from https://stackoverflow.com/q/17415499/11996851 */
static int searchStringWithKnuthMorrisPratt(STRING s, STRING t)
{
    size_t m = strlen(s);
    size_t n = strlen(t);
    int i=0,j=0,
            k=0
    ;
    int* B = malloc(sizeof(int) * (m+1));
    B[0]=-1; B[1]=0;
    for (int l=2; l<=m; l++)
    {
        while ((k>=0) && s[k] != s[l - 1]) k=B[k];
        B[l]=++k;
    }
    while (i<=(n-m))
    {
        while ((j<m) && (s[j] == t[i+j])) j++;
        if (j==m) return(i);
        i=i+j-B[j];
        j=max_nbr(0, B[j]);
    }
    return(-1);
}

size_t countOccurrencesOfSubstring(STRING substring, STRING big) {
    size_t count = 0;
    int result = searchStringWithKnuthMorrisPratt(substring, big);
    while (result > -1) {
        ++count;
        big = big + result + 2;
        result = searchStringWithKnuthMorrisPratt(substring, big);
    }
    return count;
}
