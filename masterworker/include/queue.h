#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"
#include "safe_memory.h"
#include "mutex_wrapper.h"
#include <signal.h>

extern volatile sig_atomic_t terminated;

typedef struct Queue{
    int count;
    int size;
    int numTasks; // keep track of total number of tasks (also those not in the queue)
    int isOpen; // flag used to block the execution of the tasks in the queue
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
Queue* init_queue(int size, int numTasks);

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
 * @brief blocks the queue so the workers won't receive new tasks
 */ 
void closeQueue(Queue* queue);

/**
 * @brief prints to stdout info about the queue of tasks
 * 
 * @param queue 
 */
void printQueue(Queue queue);

#endif
