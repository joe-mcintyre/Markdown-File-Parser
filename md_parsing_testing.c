#include <stdlib.h>
#include <stdio.h>
#include "md_parsing.h"

int main() {
    char* md_arr[] = { "this should be a heading", "===", "# heading", "## heading", "### heading3", "----", "egghead", "--"};
    int arr_size = sizeof(md_arr)/sizeof(md_arr[0]);

    printf("1st step\n");
    char** html_arr = convert_to_html(md_arr, arr_size);

    printf("HTML_ARR CREATED & CONVERTED\n");

    int i;
    for (i=0; i < arr_size; i++) {
        printf("LINE: %s\n", html_arr[i]);
    }

    // Open an HTML file for writing
    FILE* file = fopen("output.html", "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    // Write basic HTML structure
    fprintf(file, "<!DOCTYPE html>\n<html>\n<head>\n<title>Markdown Output</title>\n</head>\n<body>\n");

    // Write parsed HTML content
    for (int i = 0; i < arr_size; i++) {
        if (html_arr[i]) { // Ensure it's not NULL
            fprintf(file, "%s\n", html_arr[i]);
        }
    }
    // Close body and HTML tags
    fprintf(file, "</body>\n</html>\n");
    // Close the file
    fclose(file);

    printf("HTML written to output.html\n");

    return 0;
}

