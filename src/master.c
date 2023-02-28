#include "../include/master.h"

Master* init_master(Queue* queue,char** fileNames, int numTasks){
    Master* master = (Master*) safe_alloc(sizeof(Master));
    master->queue = queue;
    master->numTasks = numTasks;
    printf("%d tasks\n",master->numTasks);
    createTasksFromFiles(master, fileNames);
    return master;
}

void createTasksFromFiles(Master* master, char** fileNames){
    master->allTasks = (Task**) safe_alloc(sizeof(Task*) * master->numTasks); 
    for(int i = 0; i < master->numTasks; i++){
        (master->allTasks[i]) = init_task(fileNames[i], i);      
        printTaskInfo(*(master->allTasks[i]));
    }
}

void putNTasks(Master* master, int n){
    printf("putNTasks\n");
    for(int i = 0; i < n; i++){
        if(master->numTasks == 0) return;
        printf("putNTasks inside\n");
        insertNewTask(master->queue, removeTaskFromMaster(master));
    }
}

Task* removeTaskFromMaster(Master* master){

    printf("Removing the task from allTasks...\n");
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

