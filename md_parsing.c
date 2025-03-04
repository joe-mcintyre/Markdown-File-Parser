#include "md_parsing.h"
#include "queue.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 65536

Semantic_Parser* init_parser() {
    Semantic_Parser* new_parser = (Semantic_Parser*)malloc(sizeof(Semantic_Parser));
    Queue* new_queue = init_queue();

    new_parser->nested_attributes = new_queue;
    new_parser->active = false;
    new_parser->isHeading = false;
    return new_parser;
}

char** convert_to_html(char** md_array, int array_size) {
    //char* html_array[array_size];
    char** html_array = malloc(array_size * sizeof(char*));
                                                                                printf("DEBUG: html_array created\n");//DEBUG: html_array created
    Semantic_Parser* semantic_parser = init_parser();
                                                                                printf("DEBUG: parser created\n");//DEBUG: parser created
    bool skip_line = false;
    int i;
    for (i = 0; i < array_size; i++) {
                                                                                printf("DEBUG: %d. md_line reading\n", i);//DEBUG: parser created
        char* html_line = strdup("");
                                                                                printf("\thtml_line strdup\n");//debug: parser created
        if (skip_line) {
            skip_line = false;
        } else {
            char* line = md_array[i];
                                                                                printf("\t\tgetting previous line\n");//debug: parser created
            char* prev_line = get_prev_line(md_array, i, array_size);
                                                                                printf("\t\tgot previous line\n");//debug: parser created
                                                                                printf("\t\tgetting next line\n");//debug: parser created
            char* next_line = get_next_line(md_array, i, array_size);
                                                                                printf("\t\tgot next line\n");//debug: parser created
            bool result = underline_heading_check(semantic_parser, line, next_line, prev_line, &html_line);
            if (result) skip_line = true;
            printf("CURRENTLINE: %s\n", html_line);
        }
       html_array[i] = html_line;
                                                                                printf("\thtml_line: '%s' added to html_array\n", html_line);//debug: parser created

        // maybe do a convert line function here???, big switch case
    }
    //yada yada yada
                                                                                printf("\treturned html_array\n");//debug: parser created
    return html_array;
}

bool underline_heading_check(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char* prev_line, char** html_line) {
                                                                                printf("\tpre trimming\n");//debug: parser created
                                                                                printf("currentline: %s\n", currentline); //debug
                                                                                printf("nextline: %s\n", next_line);      //debug
                                                                                if (!currentline) {
                                                                                    printf("ERROR: currentline is NULL\n");
                                                                                    return false;
                                                                                }
                                                                                if (!next_line) {
                                                                                    printf("ERROR: next_line is NULL\n");
                                                                                    return false;
                                                                                }
    char* trimmed_currentline = trimwhitespace(strdup(currentline));
    char* trimmed_nextline = trimwhitespace(strdup(next_line));                                                                                printf("\tpost trimming\n");//debug: parser created

    if (underline_heading(trimmed_currentline, trimmed_nextline, 1)) {
                                                                                printf("\t\t\tIS HEADING 1\n");//debug: parser created
        semantic_parser->active = true;
        semantic_parser->isHeading = true;
        enqueue(semantic_parser->nested_attributes, init_node("h1"));
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        *html_line = append_strings(*html_line, "<h1>");
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        *html_line = append_strings(*html_line, currentline);
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        *html_line = append_strings(*html_line, "</h1>");
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        dequeue(semantic_parser->nested_attributes);
        return true;
    } else if (underline_heading(trimmed_currentline, trimmed_nextline, 2)) {
                                                                                printf("\tIS HEADING 2\n");//debug: parser created
        semantic_parser->active = true;
        semantic_parser->isHeading = true;
        enqueue(semantic_parser->nested_attributes, init_node("h2"));
        *html_line = append_strings(*html_line, "<h2>");
        *html_line = append_strings(*html_line, currentline);
        *html_line = append_strings(*html_line, "</h2>");
        dequeue(semantic_parser->nested_attributes);
        return true;
    }
    return false;
}

//  type should only be 1 or 2 for h1 & h2
bool underline_heading(char* line, char* next_line, int type) {
                                                                                printf("currentline: '%s'\n", line);
                                                                                printf("nextline: '%s'\n", next_line);
                                                                                printf("\tpre underline heading check\n");//debug: parser created
    int next_line_size = strlen(next_line);
                                                                                printf("nextLine: '%s' string length: %d\n", next_line, next_line_size);
    if ((type != 1 && type != 2) || line[0] == '#') return false;
    int i;
    for(i=0; i < next_line_size; i++) {
            if (type == 1 && next_line[i] != '=') return false;
            if (type == 2 && next_line[i] != '-') return false;
    }
    printf("\t\t\t\t\tMADE IT TO TRUE\n");
    return true;
}

char* parse_line(char* line) {
    return "egg";    //delete
}
char get_prev_char(char* line, int index) {
    if (index <= 0) return '\0';
    return line[index - 1];
}

char get_next_char(char* line, int index) {
    if (line == NULL || line[index] == '\0') return '\0';
    return line[index + 1] == '\0' ? '\0' : line[index + 1];
}

char* get_next_line(char** line_array, int index, int array_size) {
    if (index < 0 || index + 1 >= array_size) return NULL;
    return line_array[index + 1];
}

char* get_prev_line(char** line_array, int index, int array_size) {
    if (index <= 0 || index -1 >= array_size) return NULL;
    return line_array[index - 1];
}


char *trimwhitespace(char *str) {
    char *end;

    // Null check
    if (!str || *str == '\0')
        return str;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    end[1] = '\0';

    return str;
}


char* append_strings(const char *str1, const char *str2) {
    if (str1 == NULL || str2 == NULL) {
        return NULL;
    }
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char *result = (char *)malloc(len1 + len2 + 1); // Allocate memory for concatenated string
    
    if (result == NULL) {
        return NULL; // Return NULL if memory allocation fails
    }
    strcpy(result, str1);
    strcat(result, str2);
    
    return result; // Caller must free the allocated memory
}
