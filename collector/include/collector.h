#ifndef COLLECTOR_H
#define COLLECTOR_H
#define _GNU_SOURCE

#include "../../masterworker/include/utils.h"
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/sockfile"

/**
 * @brief Struct to represent a result implemented 
 * as a key-value pair where 
 * the filename (unique) acts as the key
 */
typedef struct Result{
    char filename[256];
    long value;
} Result;

Result createResultFromResponse(char* response);

void collectorCicle();

int getRemainingFilesNum(char* blockMessage);

int isMessageBlock(char* message);

#endif
