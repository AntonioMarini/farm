#include "../include/collector.h"
#include "unistd.h"

// Function to compare two Result structs by value (ascendending order)
int compare_values(const void* a, const void* b) {
    Result* r1 = (Result*)a;
    Result* r2 = (Result*)b;
    return r1->value - r2->value;
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
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    char buf[256];
    int numFiles;
    n = read(sockfd, buf, sizeof(buf));
    if (n == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    buf[n] = '\0';

    numFiles = isNumber(buf);
    if(numFiles == -1) {
        printf("Not a number\n");
        exit(1);
    }

    Result resultsMap[numFiles];

    for(int i = 0; i < numFiles; i++) {
        memset(buf, 0, sizeof(buf));
        n = read(sockfd, buf, sizeof(buf));
        if (n == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        buf[n] = '\0';
        resultsMap[i] = createResultFromResponse(buf);      
    }

    // Sort the map by value
    qsort(resultsMap, numFiles, sizeof(Result), compare_values);

    // Print the sorted map
    for (int i = 0; i < numFiles; i++) {
        printf("%ld: %s\n",  resultsMap[i].value, resultsMap[i].filename);
    }

    // Close the socket
    printf("Collector: bye\n");
    close(sockfd);
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
    res.value = (long) isNumber(substrings[0]);
    strcpy(res.filename,substrings[1]);

    return res;
}