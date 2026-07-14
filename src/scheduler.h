#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <sys/types.h>
#include <sys/time.h>



typedef enum{
    QUEUED,
    RUNNING,
    COMPLETED
} JobStatus;

typedef struct Job{
    int jobID;
    char jobName[100];
    char command[200];
    JobStatus status;
    pid_t pid;
    struct timeval startTime;
    struct timeval endTime;
    double executionTime;
    int exitStatus;
    struct Job *next;
} Job;

typedef struct{
    Job *front;
    Job *rear;
}Queue;

void initializeQueue(Queue *queue);
void enqueue(Queue *queue, Job *newJob);
Job* dequeue(Queue *queue);
void displayQueue(Queue *queue);

Job* createJob(int jobID, char jobName[] , char command[]);
const char* getStatusString(JobStatus status);
void executeJob(Job *job);
void logJob(Job *job);
#endif