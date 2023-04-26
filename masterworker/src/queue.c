#include "../include/queue.h"
#include <stdio.h>

Queue* init_queue(int size){
    Queue* queue = (Queue*) safe_alloc(sizeof(Queue));

    queue->isOpen = 1;
    queue->size = size;
    queue->count = 0;
    queue->tasks = (Task**) safe_alloc(sizeof(Task*) * queue->size);
    // init the mutex and condition variable
    Mutex_init(&(queue->mtx));
    pthread_cond_init(&(queue->cond_empty), NULL);
    pthread_cond_init(&(queue->cond_full), NULL);

    return queue;
}

int isQueueEmpty(Queue queue){
    return queue.count == 0;
}

int isQueueFull(Queue queue){
    return queue.count == queue.size;
}

void insertNewTask(Queue* queue, Task* task){
    Mutex_lock(&(queue->mtx));
    while(isQueueFull(*queue))
        Cond_wait(&(queue->cond_full), &(queue->mtx));
    queue->tasks[queue->count++] = task;

    Cond_signal(&(queue->cond_empty));
    Mutex_unlock(&(queue->mtx));
}

Task* removeTask(Queue* queue){
    Mutex_lock(&(queue->mtx));
    while (isQueueEmpty(*queue))
        Cond_wait(&(queue->cond_empty), &(queue->mtx));
    Task* task = queue->tasks[--queue->count];
    
    Cond_signal(&queue->cond_full);
    Mutex_unlock(&(queue->mtx));
    return task;
}

void destroyQueue(Queue* queue){
    if(queue==NULL)
        return;
    Task* t;
    while(!isQueueEmpty(*queue)){
        t=removeTask(queue);
        destroyTask(t);
    }
    safe_free(queue->tasks);
    pthread_cond_destroy(&(queue->cond_empty));
    pthread_cond_destroy(&(queue->cond_full));
    pthread_mutex_destroy(&(queue->mtx));
    safe_free(queue);
}

void printQueue(Queue queue){
    printf("There are still %d tasks:\n", queue.count);
    for(int i=0; i<queue.count; i++){
        printTaskInfo(*(queue.tasks[i]));
        printf("\n");
    }
}

void closeQueue(Queue* queue){
    queue->isOpen = 0;
}
