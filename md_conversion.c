#include "md_conversion.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 65536


char** convert_file_to_array(FILE* file) {
    int file_line_count = count_file_lines(file);
    if (file_line_count <= 0) return NULL;

    char** lines = malloc((file_line_count + 1)* sizeof(char*));
    if(!lines) return NULL;

    char buffer[BUFFER_SIZE];
    int i = 0;
    while(fgets(buffer, BUFFER_SIZE, file) && i < file_line_count) {
        lines[i] = strdup(buffer);
        // Handle if allocation fails, free all memory
        if (!lines[i]) {
            while (i > 0) free(lines[--i]); // Free previous allocations
            free(lines);
            return NULL;
        }
        i++;
    }
    lines[i] = NULL;
    return lines;
}

int count_file_lines(FILE *file) {
    char buffer[BUFFER_SIZE];
    int count = 0;
    for(;;) {
        size_t res = fread(buffer, 1, BUFFER_SIZE, file);
        if (ferror(file)) return -1;
        int i;
        for (i = 0; i < res; i++) {
            if (buffer[i] == '\n') count++;
        }
        if (feof(file)) break;
    }
    rewind(file);
    return count;
}
