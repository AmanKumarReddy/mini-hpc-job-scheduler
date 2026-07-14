#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "scheduler.h"

void initializeQueue(Queue *queue){
    queue->front = NULL;
    queue->rear  = NULL;
}

void enqueue(Queue *queue, Job *newJob){
    if(queue->rear == NULL){
        queue->rear  = newJob;
        queue->front = newJob;
        return;
    }
    queue->rear->next = newJob;
    queue->rear       = newJob;
}

Job* createJob(int jobID, char jobName[] , char command[]){
    Job *newJob = (Job*)malloc(sizeof(Job));
    if(newJob== NULL){
        printf("Memory Allcatiojn Failed!\n");
        return NULL;
    }
    newJob->pid = -1;
    newJob->startTime.tv_sec = 0;
    newJob->startTime.tv_usec = 0;
    newJob->endTime.tv_sec = 0;
    newJob->endTime.tv_usec = 0;
    newJob->executionTime = 0.0;
    newJob->exitStatus = -1;
    newJob->jobID = jobID;
    strcpy(newJob->jobName, jobName);
    strcpy(newJob->command, command);
    newJob->status = QUEUED;
    newJob->next = NULL;
    return newJob;
}

void displayQueue(Queue *queue){
    Job *current = queue->front;
    if(current == NULL){
        printf("Queue is Empty!\n");
        return;
    }
    while (current != NULL){
        printf("Job ID: %d\n", current->jobID);
        printf("Job Name: %s\n", current->jobName);
        printf("Command: %s\n", current->command);
        printf("PID : %d\n", current->pid);
        printf("Exit Code: %d\n", current->exitStatus);
        printf("Exec Time: %.9f sec\n", current->executionTime);
        printf("Status: %s\n", getStatusString(current->status));
        printf("---------------------------------------\n");
        current = current->next;
    }
}

Job* dequeue(Queue *queue){
    if(queue->front == NULL){
        return NULL;
    }
    Job *temp = queue->front;
    queue->front = queue->front->next;
    if(queue->front == NULL){
        queue->rear = NULL;
    }
    temp->next = NULL;
    return temp;
}

const char* getStatusString(JobStatus status){
    switch(status){
        case QUEUED:
            return "QUEUED";
        case RUNNING:
            return "RUNNING";
        case COMPLETED:
            return "COMPLETED";
        default:
            return "UNKNOWN";}
    }

void executeJob(Job *job){
    gettimeofday(&job->startTime, NULL);
    pid_t pid = fork();
    if(pid < 0){
        printf("Fork failed! \n");
        return;
    }
    if(pid == 0){
        execl("/bin/sh", "sh","-c",job->command, NULL);
        printf("Execution Failed\n ");
        exit(1);
    }
    else{
        job->pid = pid;
        int status;
        wait(&status);
        gettimeofday(&job->endTime, NULL);
        job->executionTime = (job->endTime.tv_sec - job->startTime.tv_sec) + (job->endTime.tv_usec - job->startTime.tv_usec)/1000000.0;
        job->exitStatus = WEXITSTATUS(status);
        job->status = COMPLETED;
        logJob(job);
    }
}

void logJob(Job *job){
    FILE *fp = fopen("../logs/jobs.log","a");
    if(fp==NULL){
        printf("Unable to open log file!\n");
        return;
    }
    fprintf(fp,"==================================================\n");
    fprintf(fp, "Job ID:   %d\n",job->jobID);
    fprintf(fp, "Job Name:   %s\n", job->jobName);
    fprintf(fp, "Command:   %s\n", job->command);
    fprintf(fp, "PID:   %d\n", job->pid);
    fprintf(fp, "Status:  %s\n", getStatusString(job->status));
    fprintf(fp, "Exit Status:  %d\n", job->exitStatus);
    fprintf(fp, "Execution Time: %.9f sec\n", job->executionTime);
    fprintf(fp, "==================================================\n\n");
    fclose(fp);
}
