#include "md_parsing.h"
#include "stack_queue.h"
//#include <cstdlib>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 65536

Semantic_Parser* init_parser() {
    Semantic_Parser* new_parser = (Semantic_Parser*)malloc(sizeof(Semantic_Parser));
    Stack_Queue* new_queue = init_queue();

    new_parser->nested_attributes = new_queue;
    new_parser->active = false;
    init_parser_bools(new_parser);
    return new_parser;
}


//normal f;i = false

void init_parser_bools(Semantic_Parser* new_parser) {
    new_parser->isH1 = false;
    new_parser->isH2 = false;
    new_parser->isH3 = false;
    new_parser->isH4 = false;
    new_parser->isH5 = false;
    new_parser->isH6 = false;
    new_parser->isUlList = false;
    new_parser->isNumList = false;
    new_parser->isCode = false;
    new_parser->isEm = false;
    new_parser->isStrong = false;
    new_parser->isEmStrong = false;
    new_parser->isURL = false;
    new_parser->isImage = false;
    new_parser->isBlockquote = false;
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
           printf("CURRENTLINE: %s\n", html_line);
           if (result) skip_line = true;
           else {
               parse_line(semantic_parser, line, next_line, prev_line, &html_line);
           }

        }
        if (semantic_parser->isH1) { printf("ish1\n"); close_heading(semantic_parser, &html_line, 1); }
        if (semantic_parser->isH2) { printf("ish2\n"); close_heading(semantic_parser, &html_line, 2); }

        html_array[i] = html_line;
    }
    //yada yada yada
                                                                                printf("\treturned html_array\n");//debug: parser created
    return html_array;
}


void parse_line(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char* prev_line, char** html_line) {
    if (currentline == NULL || currentline[0] == '\0') return;

    char* trimmed_currentline = trimwhitespace(strdup(currentline));
    char* trimmed_nextline = trimwhitespace(strdup(next_line));
    int result;
                                                                                printf("\tpost trimming\n");//debug: parser created
    switch (trimmed_currentline[0]) {
        case '#':
            result = valid_heading(semantic_parser, trimmed_currentline, html_line);
            if (result == -1) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
            } else {
                char* heading_substring = get_heading_substring(trimmed_currentline, result);
                *html_line = append_strings(*html_line, heading_substring);
                close_heading(semantic_parser, html_line, result);
            }
            
            break;

        case '*':
            break;

        case '>':
            break;

        default:
            printf("NO SPECIAL CASE STARTING LINE\n");
            break;
    }
}

void parse_line_semantics(Semantic_Parser* semantic_parser, char* currentline, char** html_line) {
    int i;
    int line_length = strlen(currentline);
    for (i=0; i < line_length; i++) {
        char curr_char = currentline[i];
        switch (curr_char) {
                case '_':   //BOLD & EM
                    //
                    break;
                case '`':   //CODE
                    break;

                case '<':   //URL
                    //gotta make sure theres no spaces before the next '>' if i want url
                    break;
                default:
                    char temp[2] = {curr_char, '\0'};  // convert char to string
                    *html_line = append_strings(*html_line, temp);
                    break;
                //![$text]($dir_path) => image
                //[$text]($link) => href_custom_url (link on text)
        }
    }
}

char* get_heading_substring(char* heading_string, int type) {
    char* output = strdup("");
    int stringlen = strlen(heading_string);
    int i;
    for (i=0; i < stringlen; i++) {
        if (i >= type) {
            char temp[2] = {heading_string[i], '\0'};  // convert char to string
            output = append_strings(output, temp);
        }
    }
    return output;
}

void close_heading(Semantic_Parser* semantic_parser, char** html_line, int type) {
    Node* stack_top;
    switch (type) {
        case 1:
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'h1'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "h1") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'h1' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                semantic_parser->isH1 = false;
                *html_line = append_strings(*html_line, "</h1>");
                pop(semantic_parser->nested_attributes);
            }
            break;
        case 2:
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'h2'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "h2") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'h2' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                semantic_parser->isH2 = false;
                *html_line = append_strings(*html_line, "</h2>");
                pop(semantic_parser->nested_attributes);
            }
            break;
        case 3:
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'h3'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "h3") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'h3' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                semantic_parser->isH3 = false;
                *html_line = append_strings(*html_line, "</h3>");
                pop(semantic_parser->nested_attributes);
            }
            break;
        case 4:
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'h4'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "h4") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'h4' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                semantic_parser->isH4 = false;
                *html_line = append_strings(*html_line, "</h4>");
                pop(semantic_parser->nested_attributes);
            }
            break;
        case 5:
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'h5'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "h5") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'h5' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                semantic_parser->isH5 = false;
                *html_line = append_strings(*html_line, "</h5>");
                pop(semantic_parser->nested_attributes);
            }
            break;
        case 6:
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'h6'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "h6") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'h6' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                semantic_parser->isH6 = false;
                *html_line = append_strings(*html_line, "</h6>");
                pop(semantic_parser->nested_attributes);
            }
            break;
        default:
            perror("Invalid operation, should not have been reached");
            exit(EXIT_FAILURE);
    }
}

int valid_heading(Semantic_Parser* semantic_parser, char* trimmed_currentline, char** html_line) {
    int x;
    for (x=1;; x++) {
        if (trimmed_currentline[x] == '\0') return -1;
        else if (trimmed_currentline[x] != '#' && trimmed_currentline[x] != ' ') return -1;
        else if (trimmed_currentline[x] == '#' && x > 6) return -1;
        if (trimmed_currentline[x] == ' ') {
            switch (x) {
                case 1:
                    semantic_parser->isH1 = true;
                    *html_line = append_strings(*html_line, "<h1>");
                    enqueue(semantic_parser->nested_attributes, init_node("h1"));
                    return 1;
                case 2:
                    semantic_parser->isH2 = true;
                    *html_line = append_strings(*html_line, "<h2>");
                    enqueue(semantic_parser->nested_attributes, init_node("h2"));
                    return 2;
                case 3:
                    semantic_parser->isH3 = true;
                    *html_line = append_strings(*html_line, "<h3>");
                    enqueue(semantic_parser->nested_attributes, init_node("h3"));
                    return 3;
                case 4:
                    semantic_parser->isH4 = true;
                    *html_line = append_strings(*html_line, "<h4>");
                    enqueue(semantic_parser->nested_attributes, init_node("h4"));
                    return 4;
                case 5:
                    semantic_parser->isH5 = true;
                    *html_line = append_strings(*html_line, "<h5>");
                    enqueue(semantic_parser->nested_attributes, init_node("h5"));
                    return 5;
                case 6:
                    semantic_parser->isH6 = true;
                    *html_line = append_strings(*html_line, "<h6>");
                    enqueue(semantic_parser->nested_attributes, init_node("h6"));
                    break;
                default:
                    perror("Invalid operation, should not have been reached");
                    exit(EXIT_FAILURE);
            }
        }
    }
    return -1;
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
        semantic_parser->isH1 = true;
        enqueue(semantic_parser->nested_attributes, init_node("h1"));
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        *html_line = append_strings(*html_line, "<h1>");
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        *html_line = append_strings(*html_line, currentline);
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        //*html_line = append_strings(*html_line, "</h1>");
                                                                                printf("html_line : '%s'\n", *html_line);  //debug
        //dequeue(semantic_parser->nested_attributes);
        return true;
    } else if (underline_heading(trimmed_currentline, trimmed_nextline, 2)) {
                                                                                printf("\tIS HEADING 2\n");//debug: parser created
        semantic_parser->active = true;
        semantic_parser->isH2 = true;
        enqueue(semantic_parser->nested_attributes, init_node("h2"));
        *html_line = append_strings(*html_line, "<h2>");
        *html_line = append_strings(*html_line, currentline);
        //*html_line = append_strings(*html_line, "</h2>");
        //dequeue(semantic_parser->nested_attributes);
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
