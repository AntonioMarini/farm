#include "main.h"
#include "constants.h"
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int numWorkers = N_WORKERS_DEFAULT;
int queueLength =  CONCURRENT_QUEUE_LENGHT_DEFAULT;
char dirPath[MAX_PATH];
int delayMillis = DEFAULT_DELAY;

int main(int argc, char *argv[])
{
    handleOptions(argc, argv);
    printf("Number of workers: %d\n", numWorkers);
    printf("Queue length: %d\n", queueLength);
    printf("Delay millis: %d\n", delayMillis);
    if(strncmp(dirPath, "", 1) != 0)
        printf("directory path: %s\n", dirPath);

    return 0;
}

void handleOptions(int argc, char *argv[]){
    int res;
    while ((res = getopt(argc, argv, ":n:q:d:t:h")) != -1)
    {
        switch(res){
            case 'n':{
                handleNumWorkersOpt(optarg);
                break;
            }
            case 'q':{
                handleQueueLengthOpt(optarg);
                break;
            }
            case 'd':{
                handleDirPathOpt(optarg);
                break;
            }
            case 't':{
                handleDelayOpt(optarg);
                break;
            }
            case 'h':{
                //TODO  make some help messages...
                printf("manual file...\n");
                break;
            }
            case ':':{
                printf("Option %c needs argument!\n", optopt);
                exit(1);
            }
            case '?':{
                printf("Option %c not recognized!\n", optopt);
                exit(1);
            }
            default:{
                break;
            }
        }
    }

    
}

void handleNumWorkersOpt(const char* opt){
    int n;
    if((n = isNumber(opt)) == -1){
        printf("Invalid option n, should be a number\n");
        exit(1);
    }
    // TODO: handle cases where the number of workers is too large (or too small)
    numWorkers = n;
}

void handleQueueLengthOpt(const char* opt){
    int q;
    if((q = isNumber(opt)) == -1){
        printf("Invalid option n, should be a number\n");
        exit(1);
    }
    // TODO: handle cases where the length is too large (or too small)
    queueLength = q;
}

void handleDelayOpt(const char* opt){
    int delay;
    if((delay = isNumber(opt)) == -1){
        printf("Invalid option n, should be a number\n");
        exit(1);
    }
    // TODO: handle cases where delay is too large (or too small)
    delayMillis = delay;
}

void handleDirPathOpt(const char* opt){
    if(strlen(opt)>MAX_PATH){
        printf("Invalid option n, should be a number\n");
        exit(1);
    }
    strncpy(dirPath, opt, sizeof(opt));
    if(!isValidDirPath(dirPath)){
        printf("%s isn't a directory path\n");
        exit(1);
    }
}

int isValidDirPath(const char* fileName)
{
    struct stat stats;

    stat(fileName, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

int isNumber(char const * value){
    char* end;
    int n = (int) strtol(value, &end, 10);
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


