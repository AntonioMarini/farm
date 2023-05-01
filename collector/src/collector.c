#include "../include/collector.h"

#include "unistd.h"
#include "../../masterworker/include/macro.h"

// Function to compare two Result structs by value (ascending order)
int compare_values(const void* a, const void* b) {
    Result* r1 = (Result*)a;
    Result* r2 = (Result*)b;

    long n1 = r1->value;
    long n2 = r2->value;

    if(n1 < n2){
        return -1;
    }else if(n1 > n2){
        return 1;
    }else{
        return 0;
    }
}

void collectorCicle(){
    int sockfd, n;
    struct sockaddr_un servaddr;

    // Create a socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SOCKET_PATH, sizeof(servaddr.sun_path) - 1);
    while (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    }

    int numFiles;
    n = read(sockfd, &numFiles, sizeof(int));
    if (n == -1) {
        perror("collector read numfiles ");
        exit(EXIT_FAILURE);
    }
    error_minusone(write(sockfd, "OK", 2), "writing number files ack", exit(EXIT_FAILURE));

    //printf("Number of files: %d\n", numFiles);

    Result resultsMap[numFiles];

    for(int i = 0; i < numFiles; i++) {
        char buf[512];
        n = read(sockfd, buf, sizeof(buf));
        if (n == -1) {
            perror("collector read message");
            exit(EXIT_FAILURE);
        }
        //fprintf(stdout, "read %s\n", buf);
        // Send acknowledgement to the server
        resultsMap[i] = createResultFromResponse(buf);
        error_minusone(write(sockfd, "OK", 2), "writing number files ack", exit(EXIT_FAILURE));
        //fprintf(stdout, "%d files processed\n", i+1);
    }

    // Sort the map by value
    qsort(resultsMap, numFiles, sizeof(Result), compare_values);

    // Print the sorted map
    for (int i = 0; i < numFiles; i++) {
        printf("%ld %s\n",  resultsMap[i].value, resultsMap[i].filename);
    }

    n = write(sockfd, "exit", 5);
    if (n == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

     // Close the socket
    close(sockfd);
}

int getRemainingFilesNum(char* blockMessage){
    char* delimiter = ":"; // The delimiter is a colon
    char* saveptr;
    char* token;
    token = strtok_r(blockMessage, delimiter, &saveptr);
    token = strtok_r(NULL, delimiter, &saveptr); //  get the number of remaining tasks

    int num = atoi(token); // Convert the token to an integer
    return num;
}

int isMessageBlock(char* message){
    char* block = "block";
    char* ptr = strstr(message, block);

    return (ptr!=NULL);
}

Result createResultFromResponse(char* response){
    Result res;

    char *token;
    char substrings[2][256];

        /* get the first token */
    token = strtok(response, " ");

    strcpy(substrings[0], token);
    token = strtok(NULL, " ");
    strcpy(substrings[1], token);
    // Get the first token

    res.value = isLongNumber(substrings[0]);

    strncpy(res.filename,substrings[1] , 256);

    return res;
}
