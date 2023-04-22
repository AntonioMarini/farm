#include "../include/worker.h"
#include <unistd.h>
#include <limits.h>

char dataToSend[256];

Worker* init_worker(Queue* queue, int id, int connfd){
    Worker* worker = safe_alloc(sizeof(Worker));
    worker->queue = queue;
    worker->id = id;
    worker->connfd = connfd;
    return worker;
}

void* worker_thread(void* arg){
    Worker* myData = (Worker*)arg;
    int shouldExit = 0;

    while(!shouldExit){
        //fprintf(stdout, "worker %d waiting for new task...\n", myData->id);
        Task* task = removeTask(myData->queue);
        //fprintf(stdout, "worker %d: processing task %d\n", myData->id, task->task_id);
        
        processTask(task, myData->connfd);
        //fprintf(stdout, "worker %d: processed ", myData->id);
        //printTaskInfo(*task);
        destroyTask(task);
        //shouldExit = 1;
    }

    return (void*) 0;
}

void processTask(Task* task, int connfd){
    char* filepath = task->filepath;
    FILE* fp = fopen(filepath, "rb");
    long count = 0;
    long number;
    int i;

    if(fp == NULL){
        perror("opening file");
        return;
    }

    while (fread(&number, sizeof(long), 1, fp) == 1) {
        // process the integer here
            count+= (number*i++);
    }
    
    memset(dataToSend, 0, sizeof(dataToSend));
    sprintf(dataToSend, "%ld %s", count, filepath);
    sendToCollector(connfd, dataToSend);

    fclose(fp);
}

void destroyWorker(Worker* worker){
    free(worker);
}

void sendToCollector(int connfd, char* message){  
    error_minusone(write(connfd, message, strlen(message) + 1), "write on socket", pthread_exit((void*) 1));
}
