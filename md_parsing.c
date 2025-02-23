#include "md_parsing.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 65536


char** convert_to_html(char** md_array) {
    char** html_array;

    int i;
    for (i = 0; i < sizeof(md_array); i++) {
        char* line = md_array[0];
        // maybe do a convert line function here???, big switch case
    }

    //yada yada yada
    return html_array;
}
