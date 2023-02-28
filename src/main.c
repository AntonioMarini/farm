#include "../include/main.h"
#include "../include/constants.h"
#include "../include/master.h"
#include "../include/safe_memory.h"

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

Queue* queue = NULL; // Queue that will be shared between master and workers.
Master* master = NULL;
char** filesList;

int main(int argc, char *argv[])
{
    //1)
    handleOptions(argc, argv);
    //checkIfFilesAreValid(dirPath);
    printOptions();

    int size  = countFilesInsideDirectory(dirPath);
    printf("Number of files: %d\n", size);
    filesList = listFilesInsideDirectory(dirPath, size);
    for(int i = 0; i < size; i++){
        printf("%s\n", filesList[i]);
    }

    // 2) init the Master data struct
    
    queue = init_queue(queueLength);
    master = init_master(queue, filesList, size);

    putNTasks(master, 4);
    Task* t = removeTask(master->queue);
    printQueue(*(master->queue));
    printf("Removed ");
    printTaskInfo(*t);
    destroyTask(t);

    // prova a riempire la coda e stamparla...
    
    // 3) init the concurrent queue
    // 4) init the workers threadpool
    // 5) fork and create the collector process
    // 6) connect the two processes using a socket connection
    // 7) start the workers threadpool

    
    cleanup();
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
        printf("Too large\n");
        exit(1);
    }
    strncpy(dirPath, opt, MAX_PATH);
    if(!isValidDirPath(dirPath)){
        printf("%s isn't a directory path\n", dirPath);
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

void printOptions(){
    printf("Number of workers: %d\n", numWorkers);
    printf("Queue max length: %d\n", queueLength);
    printf("Delay millis: %d\n", delayMillis);
    if(strncmp(dirPath, "", 1) != 0)
        printf("directory path: %s\n", dirPath);
}

char** listFilesInsideDirectory(const char* dirPath, int size){
    DIR* dir;
    struct dirent* dirFile;
    char** filesNames;

    if(strncmp(dirPath, "", 1) == 0)
        dirPath = ".";

    if((dir = opendir(dirPath)) == NULL){
        perror("Opening directory");
        exit(EXIT_FAILURE); 
    } 

    filesNames = (char**) safe_alloc(sizeof(char*) * size);
    int i = 0;
    while ((errno = 0,dirFile = readdir(dir))!=NULL) {
        if(strncmp(dirFile->d_name, ".", 1) == 0) continue;
        if(strncmp(dirFile->d_name, "..", 1) == 0) continue;
        filesNames[i] = (char*) safe_alloc(MAX_PATH);
        strncpy(filesNames[i], dirFile->d_name, MAX_PATH);
        i++;
    }
    if (errno != 0) {
        perror("Reading directory");
    }

    closeDir(dir);
    return filesNames;
}

int countFilesInsideDirectory(char* dirPath){
    DIR* dir;
    struct dirent* dirFile;

    if(strncmp(dirPath, "", 1) == 0)
        dirPath = ".";

    if((dir = opendir(dirPath)) == NULL){
        perror("Opening directory");
        exit(EXIT_FAILURE); 
    } 

    int count = 0;

    while ((errno = 0,dirFile = readdir(dir))!=NULL) {
        if(strncmp(dirFile->d_name, ".", 1) == 0) continue;
        if(strncmp(dirFile->d_name, "..", 1) == 0) continue;
        count++;
    }
    if (errno != 0) {
        perror("Reading directory");
    }

    closeDir(dir);
    return count;
}

void closeDir(DIR* dir){
    if(closedir(dir) == -1){
        perror("Closing directory");
        exit(EXIT_FAILURE); 
    } 
}

void cleanup(){
    destroy_master(master);
    free(filesList);
}