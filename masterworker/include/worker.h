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
    Queue* queue;
} Worker;

Worker* init_worker(Queue* queue, int id, int connfd);

void* worker_thread(void* arg);

void processTask(Task* task, int connfd);

void destroyWorker(Worker* worker);

char* takeOnlyFileName(char* completePath);

void sendToCollector(int connfd, char* message);


#endif
