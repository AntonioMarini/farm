#include "../include/main.h"
#include "../../collector/include/collector.h"
#include "../include/utils.h"

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
MessageBuffer* messageBuffer =NULL;

char** filesList;
int numTasks = 0;

struct pollfd pfd;
int sockfd, connfd;

pthread_mutex_t connectionMtx;

extern volatile sig_atomic_t terminated;

int main(int argc, char *argv[])
{
    //1) handle and validate options
    handleOptions(argc, argv);

    // list of all files
    listFilesInsideDirectoryRec(dirPath);

    // if list of files is empty exit
    if(numTasks == 0){
        printf("No files to process\n");
        exit(1);
    }

    //printOptions();
    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(pid == 0){
        // child: set new signal mask and start collector here
        signal(SIGINT, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTERM, SIG_IGN);
        signal(SIGHUP, SIG_IGN);

        signal(SIGPIPE, SIG_IGN);


        collectorCicle();
        for(int i = 0; i < numTasks; i++){
            safe_free(filesList[i]);
        }
        safe_free(filesList);
    }else{
        // MasterWorker must handle SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGUSR1
        setSignalHandlers();

        queue = init_queue(queueLength, numTasks);
        // parent: masterworker

        // initialize the connection
        socklen_t len;
        struct sockaddr_un servaddr, cliaddr;

         // Create a socket
        unlink(SOCKET_PATH);
        error_minusone(sockfd = socket(AF_UNIX, SOCK_STREAM, 0), "socket", exit(EXIT_FAILURE));

        // Bind the socket to a local path
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sun_family = AF_UNIX;
        strncpy(servaddr.sun_path, SOCKET_PATH, sizeof(servaddr.sun_path) - 1);
        error_minusone(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)), "bind", exit(EXIT_FAILURE));

        // Listen for incoming connections
        error_minusone(listen(sockfd, 5), "listen", exit(EXIT_FAILURE));

        // Accept incoming connections
        len = sizeof(cliaddr);
        error_minusone(connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len), "accept", exit(EXIT_FAILURE));

        //init the Master data struct
        master = init_master(filesList, queue, numTasks, delayMillis);

        //init the messagesBuffer struct
        messageBuffer = init_message_buffer();

        //init the workers threadpool
        Mutex_init(&connectionMtx);
        workers = (Worker**) safe_alloc(sizeof(Worker*) * numWorkers);
        for(int i = 0; i < numWorkers; i++){
            workers[i] = init_worker(queue, i, messageBuffer);
        }

        //start the master thread
        start_master(master);

        //start the workers threadpool
        for(int i = 0; i < numWorkers; i++)
            Thread_create(&(workers[i]->tid), worker_thread, workers[i]);

        Thread_join(master->tid, NULL);

        //printf("waiting for workers threads to terminate\n");
        for(int i = 0; i < numWorkers; i++){
            Thread_join((workers[i]->tid), NULL);
            //printf("joined worker %d\n",(int) workers[i]->tid);
        }

        //printf("all workers terminated\n");
        //printMessageBuffer(messageBuffer);

        // notify collector process about how many files do we have to work on
        Mutex_lock(&(messageBuffer->mtx));
        int totalFiles = messageBuffer->count;
        Mutex_unlock(&(messageBuffer->mtx));
        error_minusone(write(connfd, &(totalFiles), sizeof(int)), "writing to collector number of files failed",
        if(errno==EPIPE){close(connfd); close(sockfd); perror("Pipe broken:"); cleanup(); exit(EXIT_FAILURE);}
        else{cleanup();exit(EXIT_FAILURE);}); ;

        char ack[2];
        error_minusone(read(connfd, ack, sizeof(ack)), "reading number files ack failed",cleanup(); exit(EXIT_FAILURE));

        for(int i = 0; i < messageBuffer->count; i++){
            char buf[512];
            sprintf(buf,"%s", messageBuffer->messages[i]);
            error_minusone(write(connfd, buf, strlen(buf) + 1), "write on socket",
            if(errno==EPIPE){close(connfd); close(sockfd); perror("Pipe broken:"); cleanup(); exit(EXIT_FAILURE);}
            else{cleanup();exit(EXIT_FAILURE);});

            char ack[2];
            error_minusone(read(connfd, ack, sizeof(ack)), "reading ack for a message sent failed",
            if(errno==EPIPE){close(connfd); close(sockfd); perror("Pipe broken:"); cleanup(); exit(EXIT_FAILURE);}
            else{cleanup();exit(EXIT_FAILURE);});
        }

        // let server wait collector to send an exit message to terminate.
        int client_exited = 0;
        while(!client_exited){
            //printf("waiting for collector to terminate\n");
            char buf[512];
            int n = read(connfd, buf, sizeof(buf));
            if (n == -1) {
                perror("masterworker read exit");
                close(connfd);
                close(sockfd);
                cleanup();
                exit(EXIT_FAILURE);
            }
            buf[n] = '\0';

            if((strncmp(buf, "exit", 5)) == 0){
                //printf("client exited\n");
                client_exited = 1;
            }
        }

        // Close the connection and the socket
        pthread_mutex_destroy(&connectionMtx);
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

    signal(SIGPIPE, SIG_IGN);

    error_minusone(sigaction(SIGINT,&s,NULL), "sigaction", cleanup(); exit(EXIT_FAILURE));
    error_minusone(sigaction(SIGHUP,&s,NULL), "sigaction", cleanup(); exit(EXIT_FAILURE));
    error_minusone(sigaction(SIGQUIT,&s,NULL), "sigaction", cleanup(); exit(EXIT_FAILURE));
    error_minusone(sigaction(SIGTERM,&s,NULL), "sigaction", cleanup(); exit(EXIT_FAILURE));

    struct sigaction sUsr;
        memset( &sUsr, 0, sizeof(sUsr) );
        sUsr.sa_handler=gestoreUsr1;
    error_minusone(sigaction(SIGUSR1,&s,NULL), "sigaction",cleanup(); exit(EXIT_FAILURE));
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
                cleanup();
                exit(1);
            }
            case '?':{
                printf("Option %c not recognized!\n", optopt);
                cleanup();
                exit(1);
            }
            default:{
                break;
            }
        }
    }

    for (int i = 1; i < argc; i++) {
        handleFileArg(argv[i]);
    }

}

void handleNumWorkersOpt(const char* opt){
    int n;
    if((n = isNumber(opt)) == -1){
        printf("Invalid option n, should be a number\n");
        cleanup();
        exit(1);
    }
    numWorkers = n;
}

void handleQueueLengthOpt(const char* opt){
    int q;
    if((q = isNumber(opt)) == -1){
        printf("Invalid option q, should be a number\n");
        cleanup();
        exit(1);
    }
    queueLength = q;
}

void handleDelayOpt(const char* opt){
    int delay;
    if((delay = isNumber(opt)) == -1){
        printf("Invalid option t, should be a number\n");
        cleanup();
        exit(1);
    }
    delayMillis = delay;
}

void handleDirPathOpt(const char* opt){
    if(strlen(opt)>MAX_PATH){
        printf("Too large directory path\n");
        cleanup();
        exit(1);
    }
    strncpy(dirPath, opt, MAX_PATH);
    if(!isValidDirPath(dirPath)){
        printf("%s isn't a directory path\n", dirPath);
        cleanup();
        exit(1);
    }
}

void handleFileArg(char* arg){
    if(!is_regular_binary_file(arg)){
        return;
    }

    // add this to filesList
    filesList = (char**) safe_realloc((filesList), (numTasks + 1) * sizeof(char*));
    filesList[numTasks] = safe_alloc(strlen(arg) + 1);
    strncpy((filesList)[numTasks], arg, strlen(arg) + 1);
    numTasks++;

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

void listFilesInsideDirectoryRec(const char* dirPath){
    DIR* dir;
    struct dirent* dirFile;

    if(strncmp(dirPath, "", 1) == 0)
        return;

    if((dir = opendir(dirPath)) == NULL){
        perror("Opening directory");
        cleanup();
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
            listFilesInsideDirectoryRec(fullPath);
        }
        // if the entry is a file and has a .dat extension, add its name to the list
        else if (dirFile->d_type == 8) {
            numTasks++;
            filesList = (char**) safe_realloc(filesList, numTasks * sizeof(char*));
            filesList[numTasks-1] = strndup(fullPath, 255);
        }

        safe_free(fullPath);
    }

    closeDir(dir);
}

void closeDir(DIR* dir){
    if(closedir(dir) == -1){
        perror("Closing directory");
        cleanup();
        exit(EXIT_FAILURE);
    }
}

void cleanup(){
    destroy_master(master);
    destroyMessageBuffer(messageBuffer);
    destroyAllWorkers();
    safe_free(filesList);
}

// GESTORI SEGNALI

void gestore (int signum) {
    // blocca la coda dei tasks.
    closeQueue(master->queue);
}

void gestoreUsr1 (int signum) {
    // killa i workers 
    destroyAllWorkers(); 
    destroyQueue(queue);
    cleanup();
    exit(EXIT_FAILURE);
}

void destroyAllWorkers(){
    for(int i = 0; i < numWorkers; i++){
        destroyWorker(workers[i]);
    }
    safe_free(workers);
}
