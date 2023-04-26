#include "utils.h"
#include <unistd.h>

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

long isLongNumber(char const * value){
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
    
    
    return n; // valid number if end is empty
}

int is_regular_file(char *filename) {
    if (access(filename, F_OK) != 0) {
        return 0; // file does not exist
    }
    if (access(filename, R_OK) != 0) {
        return 0; // file is not readable
    }
    return 1; // file is a regular file and can be read
}

int is_binary_file(char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1; // error opening file
    }
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c < 0x20 || c > 0x7E) {
            fclose(fp);
            return 1; // binary file
        }
    }
    fclose(fp);
    return 0; // text file
}

int is_regular_binary_file(char *filename) {
    if (!is_regular_file(filename)) {
        return 0; // not a regular file
    }
    if (is_binary_file(filename)) {
        return 1; // binary file
    }
    return 0; // text file
}
