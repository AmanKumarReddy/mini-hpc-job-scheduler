#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "../common/ipc.h"
#include "scheduler.h"
#include "../common/queue.h"

int schedulerRunning = 1;

void executeJob(Job *job){
    gettimeofday(&job->startTime, NULL);
    job->status = RUNNING;
    pid_t pid = fork();
    if(pid < 0) return;
    if(pid == 0){
        FILE *fp = fopen(job->outputFile, "w");
        if (fp != NULL){
            dup2(fileno(fp), STDOUT_FILENO);
            dup2(fileno(fp), STDERR_FILENO);
            fclose(fp);
        }
        if(chdir(job->workingDirectory) != 0) exit(1);
        execl("/bin/sh", "sh", "-l", "-c", job->command, NULL);
        exit(1);
    }
    else{
        job->pid = pid;
    }
}

void logJob(Job *job){
    char *log_path = "/tmp/hpc_jobs.log";

    FILE *fp = fopen(log_path, "a");
    if(fp == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    if(timestamp) timestamp[strlen(timestamp)-1] = '\0';
    
    fprintf(fp,"==================================================\n");
    fprintf(fp, "Timestamp:      %s\n", timestamp ? timestamp : "UNKNOWN");
    fprintf(fp, "Job ID:         %d\n", job->jobID);
    fprintf(fp, "PID:            %d\n", job->pid);
    fprintf(fp, "Status:         %s\n", getStatusString(job->status));
    fprintf(fp, "Exit Status:    %d\n", job->exitStatus);
    fprintf(fp, "Execution Time: %.6f sec\n", job->executionTime);
    fprintf(fp, "==================================================\n\n");
    
    fclose(fp);
}
void cleanupExpiredJobs(Queue *completedQueue) {
    struct timeval now;
    gettimeofday(&now, NULL);
    Job *prev = NULL;
    Job *curr = completedQueue->front;
    while (curr != NULL) {
        double elapsed = (now.tv_sec - curr->completedTime.tv_sec) + (now.tv_usec - curr->completedTime.tv_usec) / 1000000.0;
        if (elapsed > 86400.0) {
            Job *toDelete = curr;
            if (prev == NULL) completedQueue->front = curr->next;
            else prev->next = curr->next;
            if (curr == completedQueue->rear) completedQueue->rear = prev;
            curr = curr->next;
            unlink(toDelete->outputFile);
            free(toDelete);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void startScheduler(Queue *waitingQueue, Queue *completedQueue) {
    static int globalJobID = 1;
    int fifoFD = openSchedulerFIFO();
    
    while(schedulerRunning) {
        // 1. MONITOR: Check for finished jobs
        Job *curr = waitingQueue->front;
        while(curr != NULL) {
            if(curr->status == RUNNING) {
                int status;
                pid_t result = waitpid(curr->pid, &status, WNOHANG);
                if(result > 0) { 
                    gettimeofday(&curr->endTime, NULL);
                    curr->executionTime = (curr->endTime.tv_sec - curr->startTime.tv_sec) + 
                                          (curr->endTime.tv_usec - curr->startTime.tv_usec) / 1000000.0;
                    curr->exitStatus = WEXITSTATUS(status);
                    curr->status = COMPLETED;
                    printf("Job ID: %d completed\n",curr->jobID);
                    fflush(stdout);
                    gettimeofday(&curr->completedTime, NULL);
                    logJob(curr); 
                }
            }
            curr = curr->next;
        }
        Job *prev = NULL;
        curr = waitingQueue->front;
        while(curr != NULL) {
            if(curr->status == COMPLETED) {
                Job *toMove = curr;
                if(prev == NULL) waitingQueue->front = curr->next;
                else prev->next = curr->next;
                if(curr == waitingQueue->rear) waitingQueue->rear = prev;
                
                curr = curr->next;
                toMove->next = NULL;
                enqueue(completedQueue, toMove);
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
        int isAnyJobRunning = 0;
        Job *check = waitingQueue->front;
        while(check != NULL) {
            if(check->status == RUNNING) { isAnyJobRunning = 1; break; }
            check = check->next;
        }
        
        if (!isAnyJobRunning) {
            Job *q = waitingQueue->front;
            while(q != NULL) {
                if(q->status == QUEUED) { executeJob(q); break; }
                q = q->next;
            }
        }
        cleanupExpiredJobs(completedQueue);
        char buffer[1024];
        int bytesRead = read(fifoFD , buffer , sizeof(buffer)-1);
        if(bytesRead <= 0){ usleep(10000); continue; }
        buffer[bytesRead] = '\0';
        char *op = strtok(buffer, "|");
        if (!op) continue;
        char *pid_str = strtok(NULL, "|");
        if (!pid_str) continue;
        int clientPID = atoi(pid_str);
        if(strcmp(op , "SUBMIT")==0){
            char *cwd = strtok(NULL, "|");
            char *cmd = strtok(NULL, "\0");
            if (cwd && cmd) processJob(clientPID, globalJobID++, cmd, cwd, waitingQueue, completedQueue);
        }
        else if(strcmp(op , "STATUS")==0){
            char *data = strtok(NULL, "|");
            if (data) processStatus(clientPID, atoi(data), waitingQueue , completedQueue);
        }
        else if(strcmp(op , "CANCEL")==0){
            char *data = strtok(NULL, "|");
            if (data) processCancel(clientPID, atoi(data), waitingQueue , completedQueue);
        }
        else if(strcmp(op, "OUTPUT")==0){
            char *data = strtok(NULL, "|");
            if (data) processOutput(clientPID, atoi(data), completedQueue);
        }
        else if(strcmp(op, "ADMIN_STOP")==0){
            char *uid_str = strtok(NULL, "|");
            if (uid_str && atoi(uid_str) == 0) schedulerRunning = 0;
        }
        else if(strcmp(op, "ADMIN_STATUS")==0){
            char *uid_str = strtok(NULL, "|");
            if (uid_str && atoi(uid_str) == 0) displayAdminStatus(clientPID, waitingQueue, completedQueue);
        }
    }
    usleep(10000);
    close(fifoFD);
}

void displayAdminStatus(int clientPID, Queue *waitingQueue , Queue *completedQueue){
    int waitingCount = 0, runningCount = 0, completedCount = 0;
    Job *current = waitingQueue->front;
    while(current != NULL){
        if(current->status == QUEUED) waitingCount++;
        else if(current->status == RUNNING) runningCount++;
        current = current->next;
    }
    Job *compCurrent = completedQueue->front;
    while(compCurrent != NULL){
        if(compCurrent->status == COMPLETED || compCurrent->status == CANCELLED) completedCount++;
        compCurrent = compCurrent->next;
    }
    char reply_fifo[64];
    sprintf(reply_fifo, "/tmp/hpc_reply_%d", clientPID);
    int reply_fd = open(reply_fifo, O_WRONLY);
    if(reply_fd != -1){
        char response[512];
        sprintf(response, "\nWaiting: %d\nRunning: %d\nCompleted: %d\n", waitingCount, runningCount, completedCount);
        write(reply_fd, response, strlen(response));
        close(reply_fd);
    }
}