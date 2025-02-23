#ifndef MD_PARSING_H
#define MD_PARSING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Main function used to convert array of 
 * md lines to array of appropriately
 * corresponding html attributes
 */
char** convert_to_html(char** md_array);

#endif
