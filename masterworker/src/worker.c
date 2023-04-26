#include "../include/worker.h"
#include <unistd.h>
#include <limits.h>

char dataToSend[256];
pthread_mutex_t mtxSocket; 

Worker* init_worker(Queue* queue, int id, int connfd, pthread_mutex_t connectionMtx){
    Worker* worker = safe_alloc(sizeof(Worker));
    worker->queue = queue;
    worker->id = id;
    worker->connfd = connfd;
    worker->connectionMtx = connectionMtx;
    return worker;
}

void* worker_thread(void* arg){
    Worker* myData = (Worker*)arg;
    while(1){
        //fprintf(stdout, "worker %d waiting for new task...\n", myData->id);
        Task* task = removeTask(myData->queue);
        //fprintf(stdout, "worker %d: processing task %d\n", myData->id, task->task_id);
        
        processTask(task, myData->connfd, myData->connectionMtx);
        //fprintf(stdout, "worker %d: processed ", myData->id);
        //printTaskInfo(*task);
        destroyTask(task);
    }

    return (void*) 0;
}

void processTask(Task* task, int connfd, pthread_mutex_t connectionMtx){
    char* filepath = task->filepath;
    FILE* fp = fopen(filepath, "rb");
    long count = 0;
    long number = 0;
    long i = 0;

    if(fp == NULL){
        perror("opening file");
        return;
    }

    while (fread(&number, sizeof(long), 1, fp) == 1) {
        // process the long here
       // printf("count += (%ld * %ld)= ", number, i);
        count+= (number*(i++));
    }
    
    memset(dataToSend, 0, sizeof(dataToSend));
    sprintf(dataToSend, "%ld %s", count, filepath);
    sendToCollector(connfd, dataToSend, connectionMtx);

    fclose(fp);
}

void destroyWorker(Worker* worker){
    safe_free(worker);
}

void sendToCollector(int connfd, char* message, pthread_mutex_t connectionMtx){  
    Mutex_lock(&connectionMtx);
    error_minusone(write(connfd, message, strlen(message) + 1), "write on socket", pthread_exit((void*) 1));
    Mutex_unlock(&connectionMtx);
}
