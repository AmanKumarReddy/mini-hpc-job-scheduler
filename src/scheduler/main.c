#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "scheduler.h"
#include "../common/queue.h"
#include "../common/ipc.h"

int main(void) {
    if (getuid() != 0) {
        fprintf(stderr, "Error: Only the administrator (root/sudo) can start the HPC scheduler.\n");
        return 1;
    }

    Queue waitingQueue;
    Queue completedQueue;
    initializeQueue(&waitingQueue);
    initializeQueue(&completedQueue);
    createSchedulerFIFO();

    printf("\n============================================================\n");
    printf("     MINI HPC JOB SCHEDULER STARTED\n");
    printf("Waiting for jobs...\n");
    printf("=============================================================\n");

    startScheduler(&waitingQueue, &completedQueue);
    return 0;
}