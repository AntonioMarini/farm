#include "../include/main.h"
#include "../../collector/include/collector.h"

#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>

int numWorkers = N_WORKERS_DEFAULT;
int queueLength =  CONCURRENT_QUEUE_LENGHT_DEFAULT;
char dirPath[MAX_PATH];
int delayMillis = DEFAULT_DELAY;

Master* master = NULL;
Worker** workers;
Queue* queue = NULL;

char** filesList;

struct pollfd pfd;

int main(int argc, char *argv[])
{
    //1)
    handleOptions(argc, argv);
    //checkIfFilesAreValid(dirPath);
    //printOptions();

    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(pid == 0){
        // child: just start collector here
        collectorCicle();
        
    }else{
        // MasterWorker must handle SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGUSR1
        setSignalHandlers();

        queue = init_queue(queueLength);
        // parent: masterworker

        // initialize the connection 
        int sockfd, connfd;
        socklen_t len;
        struct sockaddr_un servaddr, cliaddr;

         // Create a socket
        error_minusone(sockfd = socket(AF_UNIX, SOCK_STREAM, 0), "socket", exit(EXIT_FAILURE));

        // Bind the socket to a local path
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sun_family = AF_UNIX;
        strncpy(servaddr.sun_path, SOCKET_PATH, sizeof(servaddr.sun_path) - 1);
        unlink(SOCKET_PATH);
        error_minusone(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)), "bind", exit(EXIT_FAILURE));
        
        // Listen for incoming connections
        error_minusone(listen(sockfd, 5), "listen", exit(EXIT_FAILURE));

        // Accept incoming connections
        len = sizeof(cliaddr);
        error_minusone(connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len), "accept", exit(EXIT_FAILURE));
        printf("Connected\n");

        // Receive data from the client
        
        int size  = 0;
        listFilesInsideDirectoryRec(dirPath,&filesList, &size);

        // notify collector process about how many files do we have to work on
        char buf[256];
        sprintf(buf,"%d", size);
        int n = write(connfd, buf, strlen(buf));
        if(n == -1){
            perror("write to socket");
            exit(EXIT_FAILURE);
        }

        // 2) init the Master data struct
        master = init_master(filesList, queue, size);
        
        // 3) init the workers threadpool
        workers = (Worker**) safe_alloc(sizeof(Worker*) * numWorkers);
        for(int i = 0; i < numWorkers; i++){
            workers[i] = init_worker(queue, i, connfd);
        }

        // 4) start the master thread
        start_master(master);

                // 5) start the workers threadpool

        for(int i = 0; i < numWorkers; i++)
            Thread_create(&(workers[i]->tid), worker_thread, workers[i]);

            // monitor the socket for events
    pfd.fd = connfd;
    pfd.events = POLLIN | POLLHUP | POLLERR;
    while (1) {
        // wait for an event
        if (poll(&pfd, 1, -1) == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (pfd.revents & POLLHUP) {
            // client has closed the connection
            printf("Client closed the connection.\n");
            close(connfd);
            break;
        } else if (pfd.revents & POLLERR) {
            // an error has occurred
            printf("Socket error.\n");
            exit(EXIT_FAILURE);
        }
    }

        Thread_join(master->tid, NULL);

        // Close the connection and the socket
        close(connfd);
        close(sockfd);

        //printQueue(*queue);
        cleanup();
    }

    return 0;
}

void setSignalHandlers(){
    struct sigaction s; 
        memset( &s, 0, sizeof(s) );
        s.sa_handler=gestore; 

    error_minusone(sigaction(SIGINT,&s,NULL), "sigaction", exit(EXIT_FAILURE));
    error_minusone(sigaction(SIGHUP,&s,NULL), "sigaction", exit(EXIT_FAILURE));
    error_minusone(sigaction(SIGQUIT,&s,NULL), "sigaction", exit(EXIT_FAILURE));
    error_minusone(sigaction(SIGTERM,&s,NULL), "sigaction", exit(EXIT_FAILURE));

    struct sigaction sUsr; 
        memset( &sUsr, 0, sizeof(sUsr) );
        sUsr.sa_handler=gestoreUsr1; 
    error_minusone(sigaction(SIGUSR1,&s,NULL), "sigaction", exit(EXIT_FAILURE));
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

void printOptions(){
    printf("Number of workers: %d\n", numWorkers);
    printf("Queue max length: %d\n", queueLength);
    printf("Delay millis: %d\n", delayMillis);
    if(strncmp(dirPath, "", 1) != 0)
        printf("directory path: %s\n", dirPath);
}

void listFilesInsideDirectoryRec(const char* dirPath, char*** filesList,int* count){
    DIR* dir;
    struct dirent* dirFile;

    if(strncmp(dirPath, "", 1) == 0)
        dirPath = ".";

    if((dir = opendir(dirPath)) == NULL){
        perror("Opening directory");
        exit(EXIT_FAILURE); 
    } 
    errno = 0;
    while ((dirFile = readdir(dir))!=NULL) {
        if(strncmp(dirFile->d_name, ".", 1) == 0) continue;
        if(strncmp(dirFile->d_name, "..", 1) == 0) continue;

        char* fullPath = (char*) safe_alloc(strlen(dirPath) + strlen(dirFile->d_name) + 2);
        fullPath[0] = '\0';
        strncat(fullPath, dirPath, MAX_PATH);
        strncat(fullPath, "/", MAX_PATH);
        strncat(fullPath, dirFile->d_name, MAX_PATH);

        // if the entry is a directory, recursively traverse it
        if (dirFile->d_type == 4) {
            listFilesInsideDirectoryRec(fullPath, filesList, count);
        }
        // if the entry is a file, add its name to the list
        else if (dirFile->d_type == 8) {
            (*filesList) = (char**) safe_realloc((*filesList), (*count + 1) * sizeof(char*));
            (*filesList)[*count] = safe_alloc(strlen(fullPath) + 1);
            strncpy((*filesList)[*count], fullPath, strlen(fullPath) + 1);
            (*count)++;           
        }

        free(fullPath);
    }
    if (errno != 0) {
        perror("Reading directory");
        exit(1);
    }

    closeDir(dir);
}

void closeDir(DIR* dir){
    if(closedir(dir) == -1){
        perror("Closing directory");
        exit(EXIT_FAILURE); 
    } 
}

void cleanup(){
    destroy_master(master);
    destroyAllWorkers(); 
    free(filesList);
}

// GESTORI SEGNALI

/* un gestore piuttosto semplice */
void gestore (int signum) {
    printf("Ricevuto segnale %d\n",signum);


    // killa i threads 
    destroyAllWorkers(); 
    destroyQueue(queue);
    cleanup();
    exit(EXIT_FAILURE);
}

/* un gestore piuttosto semplice */
void gestoreUsr1 (int signum) {
    printf("Ricevuto segnale %d\n",signum);
    // killa i threads 
    destroyAllWorkers(); 
    destroyQueue(queue);
    cleanup();
    exit(EXIT_FAILURE);
}


void destroyAllWorkers(){
    for(int i = 0; i < numWorkers; i++){
        error_minusone(pthread_cancel(workers[i]->tid), "kill thread", exit(EXIT_FAILURE));
        destroyWorker(workers[i]);
    }
    free(workers);
}
