#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../scheduler/scheduler.h"
#include "../common/queue.h"
#include "../common/job.h"

void processOutput(int clientPID, int jobID, Queue *completedQueue) {
    char reply_fifo[64];
    sprintf(reply_fifo, "/tmp/hpc_reply_%d", clientPID);
    int reply_fd = open(reply_fifo, O_WRONLY);
    if (reply_fd == -1) return;

    char response[1024];
    int found = 0;

    Job *prev = NULL;
    Job *current = completedQueue->front;
    while (current != NULL) {
        if (current->jobID == jobID) {
            found = 1;
            FILE *fp = fopen(current->outputFile, "r");
            if (fp == NULL) {
                sprintf(response, "Error: Output file not found for Job %d.\n", jobID);
                write(reply_fd, response, strlen(response));
            } else {
                sprintf(response, "========== Output ==========\n");
                write(reply_fd, response, strlen(response));
                char ch;
                int idx = 0;
                while ((ch = fgetc(fp)) != EOF) {
                    response[idx++] = ch;
                    if (idx == sizeof(response) - 1) {
                        response[idx] = '\0';
                        write(reply_fd, response, idx);
                        idx = 0;
                    }
                }
                if (idx > 0) {
                    response[idx] = '\0';
                    write(reply_fd, response, idx);
                }
                sprintf(response, "\n============================\n");
                write(reply_fd, response, strlen(response));
                fclose(fp);
            }

            unlink(current->outputFile);

            if (prev == NULL) {
                completedQueue->front = current->next;
            } else {
                prev->next = current->next;
            }
            if (current == completedQueue->rear) {
                completedQueue->rear = prev;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        sprintf(response, "Job ID %d data already fetched. or either Job Id or Output doesnt exist yet!\n", jobID);
        write(reply_fd, response, strlen(response));
    }

    close(reply_fd);
}