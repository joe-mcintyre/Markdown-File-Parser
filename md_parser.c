#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "md_conversion.h"

char* convert_to_html_exten(const char* md_filename);
bool valid_arguments(int argc, char** argv);
bool only_contains_periods(const char *str);

int main(int argc, char** argv) {

    if (!valid_arguments(argc, argv)) return -1;

    char* filename = argv[1];
    char* html_filename = convert_to_html_exten(filename);
    FILE* html_file;


    //PRINT DEBUGGING PRINT DEBUGGING
    printf("FILENAME: %s\n", filename);
    printf("HTMLFILENAME: %s\n", html_filename);
    //PRINT DEBUGGING PRINT DEBUGGING

    FILE* md_file = fopen(filename, "r");
    if (!md_file) {
        perror("Error opening file");
        return -1;
    }

    int md_file_line_count = count_file_lines(md_file);
    printf("md line count %d\n", md_file_line_count);


    char **lines = convert_file_to_array(md_file);
    fclose(md_file);

    //PRINT DEBUGGING PRINT DEBUGGING
    if (!lines) {
        printf("Failed to read file\n");
        return 1;
    }

    for (int i = 0; lines[i] != NULL; i++) {
        printf("%s", lines[i]); 
        free(lines[i]); 
    }
    free(lines);
    //PRINT DEBUGGING PRINT DEBUGGING

    //html_file = fopen(html_filename, "w");

    return 0;
}

bool valid_arguments(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid argument count\n");
        return false;
    }
    char* filename = argv[1];
    if (only_contains_periods(filename)) {
        fprintf(stderr, "Invalid filename\n");
        return false;
    }
    if (access(filename, F_OK) != 0) {
        fprintf(stderr, "File cannot be found\n");
        return false;
    }
    return true;
}

bool only_contains_periods(const char *str) {
    if (str == NULL || *str == '\0') return true;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] != '.') {
            return false;
        }
    }
    return true;
}

char* convert_to_html_exten(const char* md_filename) {
    char* md_copy = strdup(md_filename);
    if (!md_copy) {
        perror("mem allocation failed");
        exit(EXIT_FAILURE);
    }
    char* filename = strtok(md_copy, ".");
    if (!filename) {
        free(md_copy);
        perror("file name does not exist");
        exit(EXIT_FAILURE);
    }

    size_t filename_size = strlen(filename) + strlen(".html") + 1;
    char* html_filename = malloc(filename_size);

    if (!html_filename) {
        perror("mem allocation failed");
        free(md_copy);
        exit(EXIT_FAILURE);
    }

    snprintf(html_filename, filename_size, "%s.html", filename);
    free(md_copy);
    return html_filename;
}

