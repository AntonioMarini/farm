#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"
#include "safe_memory.h"
#include "mutex_wrapper.h"

typedef struct Queue{
    int count;
    int size;
    pthread_mutex_t mtx;
    pthread_cond_t cond_empty;
    pthread_cond_t cond_full;
    Task** tasks;   
} Queue;

/**
 * @brief Create new empty queue
 * 
 * @return 
 */
Queue* init_queue(int size);

/**
 * @brief check if queue is empty
 * 
 * @param queue 
 * @return 1 if queue is empty, 0 otherwise
 */
int isQueueEmpty(Queue queue);

/**
 * @brief insert a new task into the queue
 * 
 * @param queue 
 * @param node 
 */
void insertNewTask(Queue* queue, Task* task);

/**
 * @brief remove a task from the queue
 * 
 * @param queue 
 */
Task* removeTask(Queue* queue);

/**
 * @brief destroys the queue, freeing up the memory
 * 
 * @param queue queue to be detroyed
 */
void destroyQueue(Queue* queue);

/**
 * @brief prints to stdout info about the queue of tasks
 * 
 * @param queue 
 */
void printQueue(Queue queue);

#endif
