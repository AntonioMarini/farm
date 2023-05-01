#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

#include "task.h"
#include "safe_memory.h"
#include "mutex_wrapper.h"
#include <signal.h>

#define MESSAGE_BUFFER_MAX_SIZE 256

typedef struct MessageBuffer{
    int count;
    pthread_mutex_t mtx;
    char** messages;
} MessageBuffer;

/**
 * @brief initiaize new message buffer
 *
 * @return
 */
MessageBuffer* init_message_buffer();

void addMessage(MessageBuffer* messageBuffer, const char* message);

int getCount(MessageBuffer* buffer);

void destroyMessageBuffer(MessageBuffer* messageBuffer);

void printMessageBuffer(MessageBuffer* messageBuffer);

#endif