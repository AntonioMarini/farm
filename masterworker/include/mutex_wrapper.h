#ifndef MUTEX_WRAPPER_H
#define MUTEX_WRAPPER_H

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void Mutex_lock(pthread_mutex_t* mtx);

void Mutex_unlock(pthread_mutex_t* mtx);

void Cond_signal(pthread_cond_t* cond);

void Cond_wait(pthread_cond_t* cond, pthread_mutex_t *mut);

void Mutex_init(pthread_mutex_t* mtx);

#endif
