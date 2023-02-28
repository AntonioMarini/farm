#include "collector.h"
#include "unistd.h"

int main(int argc, char const *argv[])
{
    //1) connect to farm server
    //2) wait server for receiving the number of results needed to be stored in the queue.
    //3) setup a queue for storing result sent from server
    //4) print all results from the queue
    collectorCicle();
    return 0;
}

void collectorCicle(){
    while(1){
        sleep(3);
        printf("Collecting...\n");
        fflush(NULL);
    }
}


