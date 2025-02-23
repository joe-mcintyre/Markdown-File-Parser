#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

char* convert_to_html_exten(const char* md_filename);
bool only_contains_periods(const char *str);


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid argument count. Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* filename = argv[1];
    if (only_contains_periods(filename)) {
        printf("Invalid filename");
        return -1;
    }

    char* html_filename = convert_to_html_exten(filename);
    FILE *html_file;

    html_file = fopen(html_filename, "w");

    printf("\n\n FILENAME: %s", filename);

    //html_output = fopen()

    return 0;
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

