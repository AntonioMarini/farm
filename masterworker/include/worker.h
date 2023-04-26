#ifndef WORKER_H
#define WORKER_H

#include "queue.h"
#include "macro.h"


/**
 * @brief data structure for representing a worker thread
 * 
 */
typedef struct Worker{
    pthread_t tid;
    int id;
    int connfd;
    pthread_mutex_t connectionMtx;
    Queue* queue;
} Worker;

/**
 * @brief initializes all fields needed for a worker
 * 
 */
Worker* init_worker(Queue* queue, int id, int connfd, pthread_mutex_t connectionMtx);

void* worker_thread(void* arg);

void processTask(Task* task, int connfd, pthread_mutex_t connectionMtx);

void destroyWorker(Worker* worker);

char* takeOnlyFileName(char* completePath);

void sendToCollector(int connfd, char* message, pthread_mutex_t connectionMtx);


#endif
