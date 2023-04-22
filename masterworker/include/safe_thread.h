#ifndef SAFE_THREAD_H
#define SAFE_THREAD_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void Thread_create(pthread_t *tid,void* (*start_fcn) (void *), void* arg);

void Thread_cancel(pthread_t tid);

void Thread_exit(void* status);

void Thread_join(pthread_t tid,void** status);

#endif