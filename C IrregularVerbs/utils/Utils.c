//
// Created by mfran on 17/01/2020.
//

#include "ftime.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Utils.h"
#include "../VerbsContainer.h"
#include "../Verb.h"

STRING CSV_FILENAME = "../rsc/verbs.csv";
#define PYTHON_SCRIPT_FILENAME "./get_verbs.py"

/* ***** STATIC FUNCTIONS DECLARATION ***** */

/**
 * Runs the Python script.
 *
 * @param script_filepath Where is the root folder of the .py and the venv.
 * @param script_filename Name of the Python script .py.
 * @return Return value of the execution command.
 */
static int _run_python_script(STRING script_filepath, STRING script_filename);

/* Code snippet adapted from https://stackoverflow.com/q/17415499/11996851 */
static int _searchStringWithKnuthMorrisPratt(STRING s, STRING t);

/**
 * Makes a string from one CSV portion, surrounded by ".
 *
 * @param pp_beginning Pointer to pointer to the first ", the pointed value will be altered.
 * @param output_buffer Pointer to the buffer to write on.
 */
static void _make_string_from_csv (WRITEABLE_STRING* pp_beginning, WRITEABLE_STRING output_buffer);

/* ***** STATIC FUNCTIONS DEFINITION ***** */

static int _run_python_script(STRING script_filepath, STRING script_filename) {
    WRITEABLE_STRING buffer = calloc(200, 1);
    int result;

    // 1 - Activate venv
    // 2 - Run script
    // 3 - Quit venv
    snprintf(
            buffer,
            200,
            "cd %s && cd && \"venv/Scripts/activate.bat\" && python %s && \"venv/Scripts/deactivate.bat\"", script_filepath, script_filename);
    result = system(buffer);
    if (result == EXIT_SUCCESS) {/*
        sprintf(buffer, "python %s", script_filename);
        result = system(buffer);
        if (result == EXIT_SUCCESS) {
            sprintf(buffer, ".\\venv\\Scripts\\deactivate.bat");
            system(buffer);
        }*/    }
    free(buffer);
    return result;
}

static int _searchStringWithKnuthMorrisPratt(STRING s, STRING t)
{
    size_t m = strlen(s);
    size_t n = strlen(t);
    int i=0,j=0,k=0;
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
        if (j==m) {
            free(B);
            return(i);
        }
        i=i+j-B[j];
        j=max_nbr(0, B[j]);
    }
    free(B);
    return(-1);
}

static void _make_string_from_csv (WRITEABLE_STRING* pp_beginning, WRITEABLE_STRING output_buffer) {
    WRITEABLE_STRING pointer_to_end;
    WRITEABLE_STRING pointer_to_beginning = *pp_beginning;
    ++pointer_to_beginning; // skip first "

    pointer_to_end = pointer_to_beginning + 1; // copy until next "
    while (*pointer_to_end != '"') {
        ++pointer_to_end;
    }
    strncpy(output_buffer, pointer_to_beginning, pointer_to_end - pointer_to_beginning);
    output_buffer[pointer_to_end - pointer_to_beginning] = '\0';

    pointer_to_beginning = pointer_to_end +1; // skip ending "
    *pp_beginning = pointer_to_beginning;
}

/* ***** PUBLIC FUNCTIONS DEFINITION ***** */

void fill_verbs_container() {

    // open file
    FILE* csv_file_stream = NULL;
    if ((csv_file_stream = fopen(CSV_FILENAME, "r")) == NULL) {
        if (
                (_run_python_script(".\\..\\rsc\\", PYTHON_SCRIPT_FILENAME) != EXIT_SUCCESS) || /* Failure of PYTHON script */
                (csv_file_stream = fopen(CSV_FILENAME, "r")) == NULL /* File still cannot be read */
                ) {
            exit(EXIT_BECAUSE_FILE_FAILURE);
        }
    }

    // prepare buffers
    const size_t LEN_OF_LIST = 500;
    Verb** verb_list = malloc(sizeof(Verb*) * LEN_OF_LIST);
    size_t BUFFERS_SIZE = 200;
    const WRITEABLE_STRING buffer = calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING pointer_to_beginning;

    WRITEABLE_STRING buffer_infinitive =  calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_translation = calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_time1 =       calloc(BUFFERS_SIZE, 1);
    WRITEABLE_STRING buffer_time2 =       calloc(BUFFERS_SIZE, 1);

    // skip first line
    while(getc(csv_file_stream) != '\n');

    // for each line, build a verb
    size_t current_index = 0;
    while (!feof(csv_file_stream) && !ferror(csv_file_stream)) {
        fgets(buffer, BUFFERS_SIZE, csv_file_stream);
        pointer_to_beginning = buffer;

        // get strings
        _make_string_from_csv(&pointer_to_beginning, buffer_infinitive);
        pointer_to_beginning += LEN_SEPARATOR;
        _make_string_from_csv(&pointer_to_beginning, buffer_time1);
        pointer_to_beginning += LEN_SEPARATOR;
        _make_string_from_csv(&pointer_to_beginning, buffer_time2);
        pointer_to_beginning += LEN_SEPARATOR;
        _make_string_from_csv(&pointer_to_beginning, buffer_translation);

        // make the verb
        verb_list[current_index++] = makeVerbFromStrings(buffer_infinitive, buffer_translation, buffer_time1, buffer_time2);

#define CLEAR_BUFFER(b) memset(b, 0, strlen(b) + 1) /* this resets the buffer to avoid memory garbage */
        CLEAR_BUFFER(buffer_infinitive);
        CLEAR_BUFFER(buffer_translation);
        CLEAR_BUFFER(buffer_time1);
        CLEAR_BUFFER(buffer_time2);
#undef CLEAR_BUFFER
    }

    // close file and other buffers
    free(buffer);
    free(buffer_infinitive);
    free(buffer_translation);
    free(buffer_time1);
    free(buffer_time2);
    fclose(csv_file_stream);

    // add verbs to container
    container_addVerbs((const Verb **) verb_list, current_index);

    // free verbs
    for (size_t i = 0; i < current_index; i++) {
        freeVerb(verb_list[i]);
    }
    free(verb_list);
}

size_t count_occurrences_of_substring(STRING substring, STRING big) {
    size_t count = 0;
    int result = _searchStringWithKnuthMorrisPratt(substring, big);
    while (result > -1) {
        ++count;
        big = big + result + 2;
        result = _searchStringWithKnuthMorrisPratt(substring, big);
    }
    return count;
}

void run_and_wait ( unsigned int milliseconds, void(* function) (va_list), ...) {
    int64_t t_c = milliseconds; /* time parameter converted */
    timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    va_list l;
    va_start(l, function);
    function(l);
    va_end(l);

    gettimeofday(&end_time, NULL);
    int64_t elapsed = timeval_difftime(&end_time, &start_time) / 1000;

    if (elapsed < t_c) {
        usleep((t_c - elapsed));
    }
}

int min_nbr_var(int n, ...) {
    va_list l;
    int result = INT_MAX;
    int cur, i;

    va_start(l, n);
    for(i=0; i<n; i++) {
        cur = va_arg(l, int);
        result = min_nbr(result, cur);
    }
    va_end(l);

    return result;
}

int max_nbr_var(int n, ...) {
    va_list l;
    int result = INT_MIN;
    int cur, i;

    va_start(l, n);
    for(i=0; i<n; i++) {
        cur = va_arg(l, int);
        result = max_nbr(result, cur);
    }
    va_end(l);

    return result;
}

int min_nbr(int a, int b) {
    return (int)fminl(a, b);
}

int max_nbr(int a, int b) {
    return (int)fmaxl(a, b);
}
