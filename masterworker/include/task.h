#ifndef TASK_H
#define TASK_H

#include <stdlib.h>

typedef struct Task
{
   int task_id;
   char* filepath;
} Task;

/**
 * @brief initializes a new task
 * 
 * @param filepath 
 * @param task_id 
 */
Task* init_task(char* filepath, int task_id);
 
/**
 * @brief delete task freeing up memory
 * 
 * @param task 
 */
void destroyTask(Task* task);

/**
 * @brief prints info about the task
 * 
 * @param task 
 */
void printTaskInfo(Task task);
 
#endif
