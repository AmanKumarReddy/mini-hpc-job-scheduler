#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../scheduler/scheduler.h"
#include "../common/queue.h"
#include "../common/job.h"

void processJob(int clientPID, int jobID, char command[], char workingDirectory[], Queue *waitingQueue, __attribute__((unused)) Queue *completedQueue) {
    Job *newJob = (Job *)malloc(sizeof(Job));
    if (newJob == NULL) return;
    newJob->jobID = jobID;
    strncpy(newJob->command, command, sizeof(newJob->command) - 1);
    newJob->command[sizeof(newJob->command) - 1] = '\0';
    strncpy(newJob->workingDirectory, workingDirectory, sizeof(newJob->workingDirectory) - 1);
    newJob->workingDirectory[sizeof(newJob->workingDirectory) - 1] = '\0';
    snprintf(newJob->outputFile, sizeof(newJob->outputFile), "/tmp/hpc_output_%d.txt", jobID);
    newJob->status = QUEUED;
    newJob->pid = -1;
    newJob->exitStatus = -1;
    newJob->executionTime = 0.0;
    newJob->next = NULL;
    enqueue(waitingQueue, newJob);
    printf("Job ID: %d received\n", jobID);
    fflush(stdout);
    char reply_fifo[64];
    sprintf(reply_fifo, "/tmp/hpc_reply_%d", clientPID);
    int reply_fd = open(reply_fifo, O_WRONLY);
    if (reply_fd != -1) {
        char response[128];
        sprintf(response, "Job ID: %d\n", jobID);
        write(reply_fd, response, strlen(response));
        close(reply_fd);
    }
}