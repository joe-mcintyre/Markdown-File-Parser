//hydrahd.me PRIVACY //hanime.tv

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
    Semantic_Parser* semantic_parser = init_parser();
    bool skip_line = false;
    int i;
    for (i = 0; i < array_size; i++) {
        
        char* html_line = strdup("");
        if (skip_line) {
           skip_line = false;
        } else {
           char* line = md_array[i];
           char* next_line = get_next_line(md_array, i, array_size);
           bool result = underline_heading_check(semantic_parser, line, next_line, &html_line);
           if (result) skip_line = true;
           else {
                parse_line(semantic_parser, line, next_line, &html_line);
                //html_line = append_strings(html_line, "<br>");
           }
        }
        if (semantic_parser->isH1) close_heading(semantic_parser, &html_line, 1);
        if (semantic_parser->isH2) close_heading(semantic_parser, &html_line, 2);

        html_array[i] = html_line;
    }
    return html_array;
}


void parse_line(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char** html_line) {
    if (currentline == NULL || currentline[0] == '\0') return;
    
    char* trimmed_currentline;
    char* trimmed_nextline;

    if (currentline == NULL) trimmed_currentline = "";
    else trimmed_currentline = trimwhitespace(strdup(currentline));

    if (next_line == NULL) trimmed_nextline = "";
    else trimmed_nextline = trimwhitespace(strdup(next_line));

    int result;
    switch (trimmed_currentline[0]) {
        case '#':
            // CHECKS IF CURRENTLY IN CODE BLOCK AND PREVENTS NEW SEMANTIC CREATION
            if (semantic_parser->isCode == true) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
                break;
            }
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
            // CHECKS IF CURRENTLY IN CODE BLOCK AND PREVENTS NEW SEMANTIC CREATION
            if (semantic_parser->isCode == true) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
                break;
            }
            result = valid_ullist(semantic_parser, trimmed_currentline, html_line);
            if (result == -1) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
            } else {
                char* list_substring = get_list_substring(trimmed_currentline);
                *html_line = append_strings(*html_line, list_substring);
                close_list(semantic_parser, html_line);
                check_list_state(semantic_parser, html_line, trimmed_nextline);
            }
            break;

        case'0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':
            // CHECKS IF CURRENTLY IN CODE BLOCK AND PREVENTS NEW SEMANTIC CREATION
            if (semantic_parser->isCode == true) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
                break;
            }
            result = valid_numlist(semantic_parser, trimmed_currentline, html_line);
            if (result == -1) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
            } else {
                char* list_substring = get_numlist_substring(trimmed_currentline);
                *html_line = append_strings(*html_line, list_substring);
                close_list(semantic_parser, html_line);
                check_numlist_state(semantic_parser, html_line, trimmed_nextline);
            }
            break;

        case '>':
            // CHECKS IF CURRENTLY IN CODE BLOCK AND PREVENTS NEW SEMANTIC CREATION
            if (semantic_parser->isCode == true) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
                break;
            }
            result = valid_blockquote(semantic_parser, trimmed_currentline, html_line);
            if (result == -1) {
                parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
            } else {
                char* blockquote_substring = get_blockquote_substring(trimmed_currentline, result);
                *html_line = append_strings(*html_line, blockquote_substring);
                close_blockquote(semantic_parser, html_line, result);
            }
            break;

        default:
            parse_line_semantics(semantic_parser, trimmed_currentline, html_line);
            break;
    }
    //free(trimmed_currentline);
    //free(trimmed_nextline);
}

char* get_blockquote_substring(char* list_string, int type) {
    char* output = strdup("");
    int stringlen = strlen(list_string);
    if (stringlen < 2) {
        fprintf(stderr, "ERROR: list_substring length is less then required value\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i=0; i < stringlen; i++) {
        if (i > type) {
            char temp[2] = {list_string[i], '\0'};
            output = append_strings(output, temp);
        }
    }
    return output;
}

int valid_blockquote(Semantic_Parser* semantic_parser, char* trimmed_currentline, char** html_line) {
    if (trimmed_currentline == NULL) return -1;
    int bq_indent = 0;
    int line_length = strlen(trimmed_currentline);
    if (line_length < 2) return -1;
    int i;
    for (i=0; i < line_length; i++) {
        if (trimmed_currentline[i] == ' ') break;
        else if (trimmed_currentline[i] == '>') bq_indent++;
        else return -1;
    }
    if (bq_indent == 0) return -1; //return false if no '>' found
    if (semantic_parser->isBlockquote == false) semantic_parser->isBlockquote = true;
    for (i=0; i < bq_indent; i++) {
        *html_line = append_strings(*html_line, "<blockquote>");
        enqueue(semantic_parser->nested_attributes, init_node("blockquote"));
    }
    return bq_indent;
}

void close_blockquote(Semantic_Parser* semantic_parser, char** html_line, int type) {
    int i;
    for (i=0; i < type; i++) {
        Node* stack_top;
        stack_top = get_top(semantic_parser->nested_attributes);
        if (stack_top == NULL) {
            fprintf(stderr, "ERROR: semantic stack is empty, expected 'blockquote'\n");
            exit(EXIT_FAILURE);
        } else if (strcmp(stack_top->data, "blockquote") != 0) {
            fprintf(stderr, "Incorrect semantic ordering, expected 'blockquote' recieved: %s\n", stack_top->data);
            exit(EXIT_FAILURE);
        } else {
            *html_line = append_strings(*html_line, "</blockquote>");
            pop(semantic_parser->nested_attributes);
        }
    }
}

bool char_is_int(char input) {
    if (input == '1' || input == '2' || input == '3' ||
    input == '4' || input == '5' || input == '6' || input == '7' || 
    input == '8' || input == '9' || input == '0' ) return true;
    return false;
}

bool char_is_alpha(char input) {
    return (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z');
}

int valid_numlist(Semantic_Parser* semantic_parser, char* trimmed_currentline, char** html_line) {
    if (trimmed_currentline == NULL) return -1;
    int line_length = strlen(trimmed_currentline);
    if (line_length < 2) return -1;
    int i;
    for (i=0; i < line_length; i++) {
        if (trimmed_currentline[i] == ' ' || char_is_alpha(trimmed_currentline[i])) return -1;
        if (trimmed_currentline[i] == '.' && i > 0 && char_is_int(trimmed_currentline[i-1])) {
            if (semantic_parser->isNumList == false) {
                semantic_parser ->isNumList = true;
                *html_line = append_strings(*html_line, "<ol>");
                enqueue(semantic_parser->nested_attributes, init_node("ol"));
            }
            *html_line = append_strings(*html_line, "<li>");
            enqueue(semantic_parser->nested_attributes, init_node("li"));
            return 0;
        }
    }
    return -1;
}
char* get_numlist_substring(char* list_string) {
    bool space_found = false; //used to track until space separating list found
    char* output = strdup("");
    int stringlen = strlen(list_string);
    if (stringlen < 2) {
        fprintf(stderr, "ERROR: list_substring length is less then required value\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i=0; i < stringlen; i++) {
        if (space_found) {
            char temp[2] = {list_string[i], '\0'};
            output = append_strings(output, temp);
        } else {
            if (list_string[i] == ' ') space_found = true;
        }
    }
    return output;
}

void check_numlist_state(Semantic_Parser* semantic_parser, char** html_line, char* next_line) {
    if (next_line != NULL) {
        int next_linelen = strlen(next_line);
        if (next_linelen >= 2) {
            int i;
            for (i=0; i < next_linelen; i++) {
                if (next_line[i] == ' ' || char_is_alpha(next_line[i])) break;
                if (next_line[i] == '.' && i > 0 && char_is_int(next_line[i-1])) return;
                else if (i == next_linelen-1) break;
            }
        }
    }
    Node* stack_top;
    stack_top = get_top(semantic_parser->nested_attributes);
    if (stack_top == NULL) {
        fprintf(stderr, "ERROR: semantic stack is empty, expected 'ol'\n");
        exit(EXIT_FAILURE);
    } else if (strcmp(stack_top->data, "ol") != 0) {
        fprintf(stderr, "Incorrect semantic ordering, expected 'ol' recieved: %s\n", stack_top->data);
        exit(EXIT_FAILURE);
    } else {
        semantic_parser->isNumList = false;
        *html_line = append_strings(*html_line, "</ol>");
        pop(semantic_parser->nested_attributes);
    }
}

int valid_ullist(Semantic_Parser* semantic_parser, char* trimmed_currentline, char** html_line) {
    if (trimmed_currentline[1] == '\0') return -1;
    else if (trimmed_currentline[1] != ' ') return -1;
    else {
        if (semantic_parser->isUlList == false) {
            semantic_parser->isUlList = true;
            *html_line = append_strings(*html_line, "<ul>");
            enqueue(semantic_parser->nested_attributes, init_node("ul"));
        }
        *html_line = append_strings(*html_line, "<li>");
        enqueue(semantic_parser->nested_attributes, init_node("li"));
        return 0;
    }
}

char* get_list_substring(char* list_string) {
    char* output = strdup("");
    int stringlen = strlen(list_string);
    if (stringlen < 2) {
        fprintf(stderr, "ERROR: list_substring length is less then required value\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i=0; i < stringlen; i++) {
        if (i >= 2) {
            char temp[2] = {list_string[i], '\0'};  // convert char to string
            output = append_strings(output, temp);
        }
    }
    return output;
}

void close_list(Semantic_Parser *semantic_parser, char **html_line) {
    Node* stack_top;
    stack_top = get_top(semantic_parser->nested_attributes);
    if (stack_top == NULL) {
        fprintf(stderr, "ERROR: semantic stack is empty, expected 'li'\n");
        exit(EXIT_FAILURE);
    } else if (strcmp(stack_top->data, "li") != 0) {
        fprintf(stderr, "Incorrect semantic ordering, expected 'li' recieved: %s\n", stack_top->data);
        exit(EXIT_FAILURE);
    } else {
        *html_line = append_strings(*html_line, "</li>");
        pop(semantic_parser->nested_attributes);
    }
}


void check_list_state(Semantic_Parser* semantic_parser, char** html_line, char* next_line) {
    if (next_line != NULL) {
        int next_linelen = strlen(next_line);
        if (next_linelen >= 2) {
            if (next_line[0] == '*' && next_line[1] == ' ') return;
        }
    }
    Node* stack_top;
    stack_top = get_top(semantic_parser->nested_attributes);
    if (stack_top == NULL) {
        fprintf(stderr, "ERROR: semantic stack is empty, expected 'ul'\n");
        exit(EXIT_FAILURE);
    } else if (strcmp(stack_top->data, "ul") != 0) {
        fprintf(stderr, "Incorrect semantic ordering, expected 'ul' recieved: %s\n", stack_top->data);
        exit(EXIT_FAILURE);
    } else {
        semantic_parser->isUlList = false;
        *html_line = append_strings(*html_line, "</ul>");
        pop(semantic_parser->nested_attributes);
    }
}


char* get_heading_substring(char* heading_string, int type) {
    char* output = strdup("");
    int stringlen = strlen(heading_string);
    int i;
    for (i=0; i < stringlen; i++) {
        if (i >= type+1) {  //this +1 could cause problems later
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

bool underline_heading_check(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char** html_line) {
    if (!currentline) {
        return false;
    }
    if (!next_line) {
        return false;
    }
    char* trimmed_currentline = trimwhitespace(strdup(currentline));
    char* trimmed_nextline = trimwhitespace(strdup(next_line));

    if (underline_heading(trimmed_currentline, trimmed_nextline, 1)) {
        semantic_parser->active = true;
        semantic_parser->isH1 = true;
        enqueue(semantic_parser->nested_attributes, init_node("h1"));
        *html_line = append_strings(*html_line, "<h1>");
        *html_line = append_strings(*html_line, currentline);
        return true;
    } else if (underline_heading(trimmed_currentline, trimmed_nextline, 2)) {
        semantic_parser->active = true;
        semantic_parser->isH2 = true;
        enqueue(semantic_parser->nested_attributes, init_node("h2"));
        *html_line = append_strings(*html_line, "<h2>");
        *html_line = append_strings(*html_line, currentline);
        return true;
    }
    return false;
}

//  type should only be 1 or 2 for h1 & h2
bool underline_heading(char* line, char* next_line, int type) {
    int next_line_size = strlen(next_line);
    if (next_line_size == 0) return false;
    if ((type != 1 && type != 2) || line[0] == '#') return false;
    int i;
    for(i=0; i < next_line_size; i++) {
            if (type == 1 && next_line[i] != '=') return false;
            if (type == 2 && next_line[i] != '-') return false;
    }
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



void parse_line_semantics(Semantic_Parser* semantic_parser, char* currentline, char** html_line) {
    int line_length = strlen(currentline);
    int chars_to_skip = 0;
    int i;
    for (i=0; i < line_length; i++) {
        char curr_char = currentline[i];
        switch (curr_char) {
                case '_':   //STRONG & EM
                    if (semantic_parser->isCode == true || semantic_parser->isURL == true) {
                        char temp[2] = {curr_char, '\0'};  // convert char to string
                        *html_line = append_strings(*html_line, temp);
                        break;
                    }
                    chars_to_skip = parse_strong_em(semantic_parser, currentline, html_line, '_', &i);
                    if (chars_to_skip == -1) { //skip semantic if in code mode
                        char temp[2] = {curr_char, '\0'};  // convert char to string
                        *html_line = append_strings(*html_line, temp);
                    } else {
                        i += chars_to_skip;
                    }
                    //
                    break;

                case '*': //EM & STRONG ASWELl
                    if (semantic_parser->isCode == true || semantic_parser->isURL == true) {
                        char temp[2] = {curr_char, '\0'};  // convert char to string
                        *html_line = append_strings(*html_line, temp);
                        break;
                    }
                    chars_to_skip = parse_strong_em(semantic_parser, currentline, html_line, '*', &i);
                    if (chars_to_skip == -1) { //skip semantic if in code mode
                        char temp[2] = {curr_char, '\0'};  // convert char to string
                        *html_line = append_strings(*html_line, temp);
                    } else {
                        i += chars_to_skip;
                    }
                    break;
                case '`':   //CODE
                    if (semantic_parser->isURL == true) {
                        char temp[2] = {curr_char, '\0'};  // convert char to string
                        *html_line = append_strings(*html_line, temp);
                        break;
                    }
                    chars_to_skip = parse_code_block(semantic_parser, currentline, html_line, &i);
                    if (chars_to_skip == -1) {
                        char temp[2] = {curr_char, '\0'};
                        *html_line = append_strings(*html_line, temp);
                    } else {
                        i += chars_to_skip;
                    }
                    break;

                case '<': case '>':   //URL
                    if (semantic_parser->isCode == true) {
                        char temp[2] = {curr_char, '\0'};  // convert char to string
                        *html_line = append_strings(*html_line, temp);
                        break;
                    }
                    chars_to_skip = parse_url_link(semantic_parser, curr_char, currentline, html_line, &i);
                    if (chars_to_skip == -1) {
                        char temp[2] = {curr_char, '\0'};
                        *html_line = append_strings(*html_line, temp);
                    } else {
                        i += chars_to_skip;
                    }
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
    //put this here to clean up semantics (just in case)
    while(true) {
        Node* stack_top = get_top(semantic_parser->nested_attributes);
        if (stack_top == NULL) break;
        char* top_value = get_node_data(stack_top);
        if (strcmp(top_value, "em") == 0 || strcmp(top_value, "strong") == 0) {
            pop(semantic_parser->nested_attributes);
            *html_line = append_strings(*html_line, "</");
            *html_line = append_strings(*html_line, top_value);
            *html_line = append_strings(*html_line, ">");
        } else {
            break;
        }
        // set everything to false to restart on next line
    }
    semantic_parser->isEm = false;
    semantic_parser->isStrong = false;
    semantic_parser->isEmStrong = false;
}

int parse_url_link(Semantic_Parser* semantic_parser, char curr_char, char* currentline, char** html_line, int *i) {
    if (curr_char == '>' && semantic_parser->isURL == true) {
        Node* stack_top;
        stack_top = get_top(semantic_parser->nested_attributes);
        if (stack_top == NULL) {
            fprintf(stderr, "ERROR: semantic stack is empty, expected 'a'\n");
            exit(EXIT_FAILURE);
        } else if (strcmp(stack_top->data, "a") != 0) {
            fprintf(stderr, "Incorrect semantic ordering, expected 'a' recieved: %s\n", stack_top->data);
            exit(EXIT_FAILURE);
        } else {
            *html_line = append_strings(*html_line, "</a>");
            semantic_parser->isURL = false;
            pop(semantic_parser->nested_attributes);
            return 0;
        }
    } else if (curr_char == '>' && semantic_parser->isURL != true) {
        return -1;
    } else if (curr_char == '<' && semantic_parser->isURL == true) {
        return -1;
    }
    int stringlen = strlen(currentline);
    int x;
    for (x=1; *i+x < stringlen; x++) {
        char next_char = currentline[*i+x];
        if (next_char == '>') {
            semantic_parser->isURL = true;
            enqueue(semantic_parser->nested_attributes, init_node("a"));
            *html_line = append_strings(*html_line, "<a>");
            return 0;
        } else if (next_char == ' ') {
            return -1;
        }
    }
    return -1;
}

int parse_code_block(Semantic_Parser* semantic_parser, char* currentline, char** html_line, int *i) {
    int x;
    int chars_to_skip = 0;
    for (x=1;x<=2;x++) {
        char next_char = currentline[*i+x];
        if (next_char == '`') {
            chars_to_skip++;
        } else {
            break;
        }
    }
    if (chars_to_skip == 2) {
        if (semantic_parser->isCode == false) {
            //char* rest_of_string = split_string(currentline, *i, chars_to_skip);
            /* removing closing bracket check because code can exist across mulitple lines */
            //if (closing_bracket_exists(rest_of_string, '`', 3)) {
                semantic_parser->isCode = true;
                enqueue(semantic_parser->nested_attributes, init_node("code"));
                *html_line = append_strings(*html_line, "<code>");
                return 2;
            //} else {
            //  return -1;
            //}
        } else {
            Node* stack_top;
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'code'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "code") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'code' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                *html_line = append_strings(*html_line, "</code>");
                pop(semantic_parser->nested_attributes);
            }
            semantic_parser->isCode = false;
            return 2;
        }
    } else {
        return -1;
    }
}

int parse_strong_em(Semantic_Parser* semantic_parser, char* currentline, char** html_line, char symbol, int *i) {
    int x;
    int chars_to_skip = 0;
    for (x=1;x<=2;x++) {
        char next_char = currentline[*i+x];
        if (next_char == symbol) {
            chars_to_skip++;
        } else {
            break;
        }
    }
    if (chars_to_skip == 0) {
        if (semantic_parser->isEm == false) {
            char* rest_of_string = split_string(currentline, *i, chars_to_skip);
            if (closing_bracket_exists(rest_of_string, symbol, 1)) {
                semantic_parser->isEm = true;
                enqueue(semantic_parser->nested_attributes, init_node("em"));
                *html_line = append_strings(*html_line, "<em>");
                return 0;
            } else {
                return -1;
            }
        } else {
            Node* stack_top;
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'em'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "em") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'em' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                *html_line = append_strings(*html_line, "</em>");
                pop(semantic_parser->nested_attributes);
            }
            semantic_parser->isEm = false;
        }
    } else if (chars_to_skip == 1) {
        if (semantic_parser->isStrong == false) {
            char* rest_of_string = split_string(currentline, *i, chars_to_skip);
            if (closing_bracket_exists(rest_of_string, symbol, 2)) {
                semantic_parser->isStrong = true;
                enqueue(semantic_parser->nested_attributes, init_node("strong"));
                *html_line = append_strings(*html_line, "<strong>");
                return 1;
            } else {
                return -1;
            }
        } else {
            Node* stack_top;
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'strong'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "strong") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'strong' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                *html_line = append_strings(*html_line, "</strong>");
                pop(semantic_parser->nested_attributes);
            }
            semantic_parser->isStrong = false;
        }

    } else if (chars_to_skip == 2) {
        if (semantic_parser->isEmStrong == false) {
            char* rest_of_string = split_string(currentline, *i, chars_to_skip);
            if (closing_bracket_exists(rest_of_string, symbol, 3)) {
                semantic_parser->isEmStrong = true;
                enqueue(semantic_parser->nested_attributes, init_node("em"));
                *html_line = append_strings(*html_line, "<em>");
                enqueue(semantic_parser->nested_attributes, init_node("strong"));
                *html_line = append_strings(*html_line, "<strong>");
                return 2;
            } else {
                return -1;
            }
        } else {
            Node* stack_top;
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'strong'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "strong") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'strong' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                *html_line = append_strings(*html_line, "</strong>");
                pop(semantic_parser->nested_attributes);
            }
            //now get rid of the em aswell
            stack_top = get_top(semantic_parser->nested_attributes);
            if (stack_top == NULL) {
                fprintf(stderr, "ERROR: semantic stack is empty, expected 'em'\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(stack_top->data, "em") != 0) {
                fprintf(stderr, "Incorrect semantic ordering, expected 'em' recieved: %s\n", stack_top->data);
                exit(EXIT_FAILURE);
            } else {
                *html_line = append_strings(*html_line, "</em>");
                pop(semantic_parser->nested_attributes);
            }
            semantic_parser->isEmStrong = false;
        }
    }
    return chars_to_skip;
}

bool closing_bracket_exists(char* string, char symbol, int amount) {
    int stringlen = strlen(string);
    int i;
    for (i=0;i<stringlen;i++) {
        if (string[i] == symbol) {
            if (amount == 1) return true;
            int x;
            for (x=1; x < amount; x++) {
                //if (string[i+x] != '\0' && string[i+x] == symbol) {
                if (string[i+x] == symbol && x == amount-1) return true;
            }
        }
    }
    return false;
}


char* split_string(char* string, int index, int skipped_chars) {
    char* output = strdup("");
    int stringlen = strlen(string);
    if (index+skipped_chars >= stringlen) {
        fprintf(stderr, "ERROR: requested split index is greater then string length\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i=0; i < stringlen; i++) {
        if (i > index+skipped_chars) {
            char temp[2] = {string[i], '\0'};
            output = append_strings(output, temp);
        }
    }
    return output;
}
