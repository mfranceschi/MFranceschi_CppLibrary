//
// Created by mfran on 17/01/2020.
//

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
    FILE* csv_file_stream = NULL;
    if ((csv_file_stream = fopen(CSV_FILENAME, "r")) == NULL) {
        if (
                system("python " PYTHON_SCRIPT_FILENAME) || /* Failure of PYTHON script */
                (csv_file_stream = fopen(CSV_FILENAME, "r")) == NULL /* File still cannot be read */
                ) {
            exit(EXIT_BECAUSE_FILE_FAILURE);
        }
    }

    // prepare buffers
    const size_t LEN_OF_LIST = 500;
    Verb** verb_list = malloc(sizeof(Verb*) * LEN_OF_LIST);
    size_t BUFFERS_SIZE = 200;
    WRITEABLE_STRING buffer_infinitive =  calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_translation = calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_time1 =       calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_time2 =       calloc(BUFFERS_SIZE, 1);

    // skip first line
    while(getc(csv_file_stream) != '\n');

    // for each line, build a verb
    size_t current_index = 0;
    while (!feof(csv_file_stream) && !(ferror(csv_file_stream))) {
        fscanf(csv_file_stream, R"format("%s","%s","%s","%s\n")format", buffer_infinitive, buffer_translation, buffer_time1, buffer_time2);
        if (*buffer_infinitive && *buffer_translation && *buffer_time1 && *buffer_time2) {
            verb_list[current_index++] = makeVerbFromStrings(buffer_infinitive, buffer_translation, buffer_time1, buffer_time2);
        }
    }

    // close file and other buffers
    fclose(csv_file_stream);
    free(buffer_infinitive);
    free(buffer_translation);
    free(buffer_time1);
    free(buffer_time2);

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
