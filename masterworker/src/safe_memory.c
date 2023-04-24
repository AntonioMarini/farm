#include "../include/safe_memory.h"
#include <stdio.h>

void* safe_alloc(size_t size){
    void *ptr;
    if ((ptr = malloc(size)) == NULL ){
        printf("Failed to allocate, aborting...\n");
        exit(EXIT_FAILURE);
    }
    return (ptr);
}

void* safe_realloc(void *ptr, size_t size){
    if((ptr = realloc(ptr, size)) == NULL){
        printf("Failed to reallocate, aborting...\n");
        exit(EXIT_FAILURE);
    }
    return (ptr);
}

void safe_free(void*pointer_ptr){
    if(pointer_ptr != NULL){
        free(pointer_ptr);
    }
}
