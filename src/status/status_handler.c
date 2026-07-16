#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../scheduler/scheduler.h"
#include "../common/queue.h"
#include "../common/job.h"

void processStatus(int clientPID, int jobID, Queue *waitingQueue, Queue *completedQueue) {
    char reply_fifo[64];
    sprintf(reply_fifo, "/tmp/hpc_reply_%d", clientPID);
    int reply_fd = open(reply_fifo, O_WRONLY);
    if (reply_fd == -1) return;

    char response[512] = ""; 
    int found = 0;
    int queuedPosition = 0;

    Job *current = waitingQueue->front;
    while (current != NULL) {
        if (current->status == QUEUED) queuedPosition++;
        if (current->jobID == jobID) {
            found = 1;
            sprintf(response, "Job ID:    %d\nStatus:    %s\nPosition:  %d\n", 
                    current->jobID, getStatusString(current->status), queuedPosition);
            break;
        }
        current = current->next;
    }

    if (!found) {
        current = completedQueue->front;
        while (current != NULL) {
            if (current->jobID == jobID) {
                found = 1;
                sprintf(response, "Job ID:    %d\nStatus:    %s\nPosition:  Not in the queue\nExit Code: %d\nRuntime:   %.6f sec\n", 
                        current->jobID, getStatusString(current->status), current->exitStatus, current->executionTime);
                break;
            }
            current = current->next;
        }
    }

    if (!found) {
        sprintf(response, "Job ID %d not found.\n", jobID);
    }
    write(reply_fd, response, strlen(response));
    close(reply_fd);
}