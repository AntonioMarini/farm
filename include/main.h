#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

/**
 * @brief handles the options passed to the program
 * 
 * @param argc the number of arguments
 * @param argv the command line arguments
 */
void handleOptions(int argc, char *argv[]);

/**
 * @brief handles the dir path option (q).
 * 
 * @param opt 
 */
void handleDirPathOpt(const char* opt);

/**
 * @brief checks if a given string is a valid pathname of a directory.
 * 
 * @return int 
 */
int isValidDirPath(const char*);

/**
 * @brief handles the number of workers option (n).
 * 
 * @param opt 
 */
void handleNumWorkersOpt(const char* opt);

/**
 * @brief handles the queue length option (q).
 * 
 * @param opt 
 */
void handleQueueLengthOpt(const char* opt);

/**
 * @brief handles the queue length option (t).
 * 
 * @param opt 
 */
void handleDelayOpt(const char* opt);

/**
 * @brief checks if a given string is a number.
 * 
 * @param value string to check
 * 
 * @return 1 if the string is a number, -1 otherwise
 */
int isNumber(const char * value);

/**
 * @brief print options received
 * 
 */
void printOptions();

/**
 * @brief creates a list of filenames inside a given directory
 * @param dirPath 
 * @return char** 
 */
char** listFilesInsideDirectory(const char* dirPath, int size);

int countFilesInsideDirectory(char* dirPath);

void closeDir(DIR* dir);

void cleanup();

#endif