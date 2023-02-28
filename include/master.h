#ifndef MASTER_H
#define MASTER_H

#include "queue.h"
#include "task.h"

/**
 * @brief data structure for representing a master thread
 * 
 */
typedef struct Master{
    Queue* queue;
    Task** allTasks;
    int numTasks;
} Master;

Master* init_master(Queue* queue, char** fileNames, int numTasks);

void createTasksFromFiles(Master* master, char** fileNames);

void putNTasks(Master* master, int n);

Task* removeTaskFromMaster(Master* master);

void destroy_master(Master* master);

#endif