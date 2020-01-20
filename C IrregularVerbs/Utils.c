//
// Created by mfran on 17/01/2020.
//

#include <stdio.h>
#include <string.h>
#include "Utils.h"

STRING CSV_FILENAME = "rsc/rsc.csv";

void fillVerbsContainer() {
    FILE* csv_file = NULL;
    // open file
    // skip first line
    // for each line, build a verb
    // close file
    // add verbs to container
    // free verbs
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
