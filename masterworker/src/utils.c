#include "utils.h"

int isNumber(char const * value){
    char* end;
    long n = strtol(value, &end, 10);
    if((errno == ERANGE && (n == LONG_MAX || n == LONG_MIN)) || (errno != 0 && n == 0)){
        perror("Invalid number");
        return -1;
    }      
    if(end==value){ // not a number
        return -1;
    }

    if(*end != '\0'){
        return -1;
    };
    
    return (int) n; // valid number if end is empty
}