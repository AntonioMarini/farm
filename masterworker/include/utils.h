#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief checks if a given string is a number.
 * 
 * @param value string to check
 * 
 * @return 1 if the string is a number, -1 otherwise
 */
int isNumber(const char * value);

/**
 * @brief checks if a given string is a long number.
 * 
 * @param value string to check
 * 
 * @return 1 if the string is a number, -1 otherwise
 */
long isLongNumber(char const * value);

int is_regular_file(char *filename);

int is_binary_file(char *filename);

int is_regular_binary_file(char *filename);

#endif
