#include "../include/mutex_wrapper.h"

void Mutex_init(pthread_mutex_t* mtx){
    int err;  
	if ( ( err=pthread_mutex_init(mtx, NULL)) != 0 ) {  
		errno=err;  
		perror("init");  
		exit(1);
	}  
}

void Mutex_lock(pthread_mutex_t* mtx){
    int err;  
	if ( ( err=pthread_mutex_lock(mtx)) != 0 ) {  
		errno=err;  
		perror("lock");  
		exit(1);
	}  
}

void Mutex_unlock(pthread_mutex_t* mtx){
    int err;  
	if ( ( err=pthread_mutex_unlock(mtx)) != 0 ) {  
		errno=err;  
		perror("unlock");  
		exit(1); 
	}  
}

void Cond_wait(pthread_cond_t* cond, pthread_mutex_t *mut){
	int err;  
	if ( ( err=pthread_cond_wait(cond, mut)) != 0 ) {  
		errno=err;
		perror("Condition wait");  
		exit(1); 
	} 
}

void Cond_signal(pthread_cond_t* cond){
	int err;  
	if ( ( err=pthread_cond_signal(cond)) != 0 ) {  
		errno=err;
		perror("Condition wait");  
		exit(1); 
	} 
}
