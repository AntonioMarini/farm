#include "../include/message_buffer.h"
#include <stdio.h>
#include <string.h>


MessageBuffer* init_message_buffer(){
    MessageBuffer* messageBuffer = (MessageBuffer*) safe_alloc(sizeof(MessageBuffer));

    messageBuffer->count = 0;
    messageBuffer->messages = (char**) safe_alloc(sizeof(char*) * MESSAGE_BUFFER_MAX_SIZE);
    // init the mutex and condition variable
    Mutex_init(&(messageBuffer->mtx));

    return messageBuffer;
}

void addMessage(MessageBuffer* messageBuffer, const char* message){
    Mutex_lock(&(messageBuffer->mtx));
    messageBuffer->messages[messageBuffer->count] = (char*) safe_alloc(strlen(message) + 1);
    strncpy(messageBuffer->messages[messageBuffer->count], message, strlen(message) + 1);
    messageBuffer->count++;
    Mutex_unlock(&(messageBuffer->mtx));
}

void destroyMessageBuffer(MessageBuffer* messageBuffer){
    int i = 0;
    while(i<messageBuffer->count &&messageBuffer->messages[i] != NULL)
        safe_free(messageBuffer->messages[i++]);
    safe_free(messageBuffer->messages);
    pthread_mutex_destroy(&(messageBuffer->mtx));
    safe_free(messageBuffer);
}

void printMessageBuffer(MessageBuffer* messageBuffer){
    for(int i = 0; i <messageBuffer->count; i++){
        fprintf(stdout, "message[%d]: %s\n", i, messageBuffer->messages[i]);
    }
}

