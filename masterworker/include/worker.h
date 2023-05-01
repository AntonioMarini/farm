#ifndef WORKER_H
#define WORKER_H

#include "queue.h"
#include "macro.h"
#include "message_buffer.h"

#include <unistd.h>
#include <limits.h>
#include <signal.h>

/**
 * @brief data structure for representing a worker thread
 * 
 */
typedef struct Worker{
    pthread_t tid;
    int id;
    MessageBuffer* message_buffer;
    Queue* queue;
} Worker;

/**
 * @brief initializes all fields needed for a worker
 * 
 */
Worker* init_worker(Queue* queue, int id, MessageBuffer* messageBuffer);

void* worker_thread(void* arg);

void processTask(Task* task, MessageBuffer* MessageBuffer);

void destroyWorker(Worker* worker);

char* takeOnlyFileName(char* completePath);

#endif
