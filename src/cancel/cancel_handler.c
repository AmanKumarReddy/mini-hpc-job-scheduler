#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "../scheduler/scheduler.h"
#include "../common/queue.h"
#include "../common/job.h"

void processCancel(int clientPID, int jobID , Queue *waitingQueue , Queue *completedQueue) {
    char reply_fifo[64];
    sprintf(reply_fifo, "/tmp/hpc_reply_%d", clientPID);
    int reply_fd = open(reply_fifo, O_WRONLY);
    if(reply_fd == -1) return;

    char response[256];
    int found = 0;

    Job *prev = NULL;
    Job *curr = waitingQueue->front;

    while (curr != NULL) {
        if (curr->jobID == jobID) {
            found = 1;
            
            if (curr->status == RUNNING) {
                kill(curr->pid, SIGKILL);
            }
            
            curr->status = CANCELLED;
            printf("Job ID: %d cancelled\n", curr->jobID);
            fflush(stdout);
            if (prev == NULL) {
                waitingQueue->front = curr->next;
            } else {
                prev->next = curr->next;
            }
            if (curr == waitingQueue->rear) {
                waitingQueue->rear = prev;
            }
            
            curr->next = NULL;
            
            enqueue(completedQueue, curr);
            logJob(curr);
            
            sprintf(response, "Job ID %d successfully cancelled.\n", jobID);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) {
        sprintf(response, "Job ID %d could not be cancelled (it may be finished or does not exist).\n", jobID);
    }

    write(reply_fd, response, strlen(response));
    close(reply_fd);
}