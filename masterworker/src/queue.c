#include "../include/queue.h"
#include <stdio.h>

extern volatile sig_atomic_t terminated = 0;


Queue* init_queue(int size, int numTasks){
    Queue* queue = (Queue*) safe_alloc(sizeof(Queue));

    queue->isOpen = 1;
    queue->size = size;
    queue->numTasks = numTasks; // number ot total tasks to process in this queue
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

    pthread_cond_broadcast(&(queue->cond_empty));
    Mutex_unlock(&(queue->mtx)); 
}

Task* removeTask(Queue* queue){
    Mutex_lock(&(queue->mtx));
    while (queue->isOpen && isQueueEmpty(*queue)){
        if(queue->numTasks<=0){
            Mutex_unlock(&(queue->mtx));
            return NULL;
        }
        Cond_wait(&(queue->cond_empty), &(queue->mtx)); // rilascia lock e si mette in attesa di cond_empty notify
    }

    if(isQueueEmpty(*queue)){ // this means that queue is closed and is empty, so we need to notify the worker giving him a null task
            fprintf(stdout,"queue is empty\n");
            Mutex_unlock(&(queue->mtx));
            return NULL;
    }

    Task* task = queue->tasks[--queue->count];
    queue->numTasks--;
    //printTaskInfo(*task); 
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
    //pthread_cond_destroy(&(queue->cond_empty));
    //pthread_cond_destroy(&(queue->cond_full));
    //pthread_mutex_destroy(&(queue->mtx));


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
    queue->numTasks = queue->count;
    pthread_cond_broadcast(&(queue->cond_empty));
}
