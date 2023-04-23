#ifndef MAIN_H
#define MAIN_H

#define _XOPEN_SOURCE 700

#include "constants.h"
#include "master.h"
#include "worker.h"
#include "safe_memory.h"
#include "macro.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>

/**
 * Sets signal handlers needed to gracefully kill the process
 */
void setSignalHandlers();

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
 * @brief print options received
 * 
 */
void printOptions();

/**
 * @brief creates a list of filenames inside a given directory, visiting it recursively
 * @return  
 */
void listFilesInsideDirectoryRec(const char* dirPath, char*** filesList ,int* count);

void closeDir(DIR* dir);

void cleanup();

void gestore (int signum);

void gestoreUsr1 (int signum);

void destroyAllWorkers();

#endif
