#include "../include/master.h"

Master* init_master(char** fileNames, Queue* queue, int numTasks){
    Master* master = (Master*) safe_alloc(sizeof(Master));
    master->queue = queue;
    master->numTasks = numTasks;
    master->tid = (pthread_t) -1;
    //printf("%d tasks\n",master->numTasks);
    createTasksFromFiles(master, fileNames);
    return master;
}

void start_master(Master* master){
    Thread_create(&(master->tid), master_thread, (void*) master);
}

void* master_thread(void* arg){
    //printf("I am master thread\n");

    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 500 * 1000000; // nanoseconds (500 milliseconds)

    Master* master = (Master*) arg;
    while(master->numTasks>0){
        putNTasks(master, 1);
        nanosleep(&sleepTime, NULL);
    }

    return (void*)0;
}

void createTasksFromFiles(Master* master, char** fileNames){
    master->allTasks = (Task**) safe_alloc(sizeof(Task*) * master->numTasks); 
    for(int i = 0; i < master->numTasks; i++){
        (master->allTasks[i]) = init_task(fileNames[i], i);      
    }
}

void putNTasks(Master* master, int n){
    for(int i = 0; i < n; i++){
        if(master->numTasks == 0) return;
        insertNewTask(master->queue, removeTaskFromMaster(master));
    }
}

Task* removeTaskFromMaster(Master* master){
    Task* t = master->allTasks[0];
    for (int i = 0; i < (master->numTasks)-1; i++) {
        master->allTasks[i] = master->allTasks[i+1];
    }
    master->numTasks -= 1;
    return t;
}

void destroy_master(Master* master){
    Task* t;
    while(master->numTasks > 0){
        t = removeTaskFromMaster(master);
        destroyTask(t);
    }
    free(master->allTasks);
    destroyQueue(master->queue);
    free(master);
}
