#ifndef WORKER_H
#define WORKER_H

#include "queue.h"

/**
 * @brief data structure for representing a worker thread
 * 
 */
typedef struct Worker{
    Queue queue;
    
} Worker;

#endif