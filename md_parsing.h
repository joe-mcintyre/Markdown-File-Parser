#ifndef MD_PARSING_H
#define MD_PARSING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "stack_queue.h"

struct parser {
   Stack_Queue* nested_attributes;
   bool active;
   bool isH1;  //1st char
   bool isH2;  //1st char
   bool isH3;  //1st char
   bool isH4;  //1st char
   bool isH5;  //1st char
   bool isH6;  //1st char
   bool isUlList;   //1st char
   bool isNumList;  //1st char
   bool isCode;
   bool isEm;
   bool isStrong;
   bool isEmStrong;
   bool isURL;
   bool isImage;
   bool isBlockquote;  //1st char
};

typedef struct parser Semantic_Parser;

/* Main function used to convert array of 
 * md lines to array of appropriately
 * corresponding html attributes
 */
Semantic_Parser* init_parser();

void init_parser_bools(Semantic_Parser* new_parser);

char** convert_to_html(char** md_array, int array_size);

char* append_strings(const char *str1, const char *str2);

void parse_line(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char* prev_line, char** html_line);

int valid_heading(Semantic_Parser* semantic_parser, char* trimmed_currentline, char** html_line);

void close_heading(Semantic_Parser* semantic_parser, char** html_line, int type);

void parse_line_semantics(Semantic_Parser* semantic_parser, char* currentline, char** html_line);

char* get_heading_substring(char* heading_string, int type);

bool underline_heading_check(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char* prev_line, char** html_line);

bool underline_heading(char* line, char* next_line, int type);

char get_prev_char(char* line, int index);

char get_next_char(char* line, int index);

char* get_next_line(char** line_array, int index, int array_size);

char* get_prev_line(char** line_array, int index, int array_size);

char *trimwhitespace(char *str);

#endif
