# C Programming

## Memory Concept
* Location (declaration), Type (declaration), Value (assignment)
## Data Types
* int:          2-4 bytes   %d
    * 4byte(32bit) int can take 2^32 distinct values between -/+2147483648
* char:         1 byte      %c
    * char has values between -128 (2^7) & 127 (2^7-1)
* unsigned int: 2-4 bytes   %u
    * 4byte(32bit) unsigned can take 2^32 distinct values from 0 to 4294967295
* long int:     4-8 bytes   %ld
* float:        4 bytes     %f
* double:       8 bytes     %lf
    * float is single precision while double is double precision
## Formatting
* %5.2f allocates 5 spaces for entire num & formats 2 digits to right of D.P


## Operator Precendence
1. ( )
2. *, /, %
3. +, -

### Conditional Operator
```
printf("%s\n", grade >= 60 ? "Passed" : "Failed" );
grade >= 60 ? printf("Passed\n") : printf("Failed\n");
```

## Static Variables
* static vars allocated & initialised once before prog execution
```static int x = 50;  //  initalized once```

## Variables across source files
```
//  file1
int number = 1234
//  file2
#include <file1.h> //works if file1 has header
int main() { extern int number;  //called from file 1
```
