#include "../include/worker.h"

char dataToSend[256];
pthread_mutex_t mtxSocket;

Worker* init_worker(Queue* queue, int id, MessageBuffer* messageBuffer){
    Worker* worker = safe_alloc(sizeof(Worker));
    worker->queue = queue;
    worker->id = id;
    worker->message_buffer = messageBuffer;
    return worker;
}

void* worker_thread(void* arg){
    Worker* myData = (Worker*)arg;
    while(1){
        //fprintf(stdout, "worker %d waiting for new task...\n", myData->id);
        Task* task = removeTask(myData->queue);
        if(task == NULL){
            return (void*) 0;
        }
        //fprintf(stdout, "worker %d: processing task %d\n", myData->id, task->task_id);

        processTask(task, myData->message_buffer);
        destroyTask(task);
    }
    //fprintf(stdout, "worker %d thread finished\n", myData->id);
    return (void*) 0;
}

void processTask(Task* task, MessageBuffer* messageBuffer){
    char* filepath = task->filepath;
    FILE* fp = fopen(filepath, "rb");
    long count = 0;
    long number = 0;
    long i = 0;

    if(fp == NULL){
        perror("opening file");
        return;
    }

        //fprintf(stdout, "reading files\n");
    while (fread(&number, sizeof(long), 1, fp) == 1) {
        // process the long here
        count+= (number*(i++));
    }

    memset(dataToSend, 0, sizeof(dataToSend));
    sprintf(dataToSend, "%ld %s", count, filepath);

    addMessage(messageBuffer, dataToSend);
    fclose(fp);
}

void destroyWorker(Worker* worker){
    safe_free(worker);
}
