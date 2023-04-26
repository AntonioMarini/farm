#ifndef MASTER_H
#define MASTER_H

#include "queue.h"
#include "task.h"
#include "safe_thread.h"
#include <time.h>

/**
 * @brief data structure for representing a master thread
 * 
 */
typedef struct Master{
    Queue* queue;
    Task** allTasks;
    int numTasks;
    int delayMillis;
    pthread_t tid;
} Master;

Master* init_master(char** fileNames, Queue* queue,int numTasks, int delayMillis);

void start_master(Master* master);

void* master_thread(void* arg);

void createTasksFromFiles(Master* master, char** fileNames);

void putNTasks(Master* master, int n);

Task* removeTaskFromMaster(Master* master);

void destroy_master(Master* master);

int nanosleep(const struct timespec *req, struct timespec *rem);

#endif
