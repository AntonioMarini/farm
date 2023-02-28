#include "../include/task.h"
#include "../include/safe_memory.h"
#include <stdio.h>

Task* init_task(char* filepath, int task_id){
    Task* task = safe_alloc(sizeof(Task));
    task->filepath = filepath;
    task->task_id = task_id;
    return task;
}

void destroyTask(Task* task){
    free(task->filepath);
    free(task);
}

void printTaskInfo(Task task){
    printf("Task %d: %s\n",task.task_id,task.filepath);
}