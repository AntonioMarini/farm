#include "safe_thread.h"

void Thread_create(pthread_t *tid,void* (*start_fcn) (void *), void* arg){
    int res = pthread_create(tid,NULL, start_fcn, arg);
    if(res != 0){
        perror("pthread_create");
        exit(1);
    }
}

void Thread_cancel(pthread_t tid){
    int res = pthread_cancel(tid);
    if(res != 0){
        perror("pthread_cancel");
        exit(1);
    }
}

void Thread_exit(void* status){
    pthread_exit(status);
}

void Thread_join(pthread_t tid,void** status){
    int res = pthread_join(tid, status);
    if(res != 0){
        perror("pthread_join");
        exit(1);
    }
}