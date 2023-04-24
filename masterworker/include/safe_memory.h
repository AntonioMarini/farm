#ifndef SAFE_MEMORY_H
#define SAFE_MEMORY_H

#include <stdlib.h>

/**
 * @brief safe wrapper around memory allocation calls
 * 
 */

/**
 * @brief safely allocates memory
 * 
 * @param size the size requested
 * @return the pointer to the first cell of allocated memory
 */
void* safe_alloc(size_t size);

/**
 * @brief safely reallocates memory with given new size
 * 
 * @param ptr 
 * @param newsize 
 * @return void* 
 */
void* safe_realloc(void* ptr, size_t newsize);

/**
 * @brief safely frees memory
 * 
 */
void safe_free(void* pointer_ptr);

#endif
