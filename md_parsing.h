#ifndef MD_PARSING_H
#define MD_PARSING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

struct parser {
   Queue* nested_attributes;
   bool active;
   bool isHeading;
   bool isUlList;
   bool isNumList;
   bool isCode;
   bool isEm;
   bool isStrong;
   bool isEmStrong;
   bool isURL;
   bool isImage;
   bool isBlockquote;
};
typedef struct parser Semantic_Parser;

/* Main function used to convert array of 
 * md lines to array of appropriately
 * corresponding html attributes
 */
char** convert_to_html(char** md_array, int array_size);

/* Parses markdown line and returns
 * it as a formatted html attribute
 * for html line array
 */

char* append_strings(const char *str1, const char *str2);

char* parse_line(char* line);

bool underline_heading_check(Semantic_Parser* semantic_parser, char* currentline, char* next_line, char* prev_line, char** html_line);

bool underline_heading(char* line, char* next_line, int type);

char get_prev_char(char* line, int index);

char get_next_char(char* line, int index);

char* get_next_line(char** line_array, int index, int array_size);

char* get_prev_line(char** line_array, int index, int array_size);

char *trimwhitespace(char *str);

#endif
