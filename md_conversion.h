#ifndef MD_CONVERSION_H
#define MD_CONVERSION_H

#include <stdio.h>

/* Takes FILE structure & places all file lines into an array
 */
char** convert_file_to_array(FILE* file);

/* Uses FILE struct parameter and returns the total
 * of lines in file
 */
int count_file_lines(FILE* file);

#endif
