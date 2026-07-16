#ifndef JOB_H
#define JOB_H

#include <sys/time.h>
#include <sys/types.h>

typedef enum {
    QUEUED,
    RUNNING,
    COMPLETED,
    CANCELLED
} JobStatus;

typedef struct Job {
    int jobID;
    char command[1024];
    char workingDirectory[512];
    char outputFile[256];
    JobStatus status;
    pid_t pid;
    int exitStatus;
    struct timeval startTime;
    struct timeval endTime;
    struct timeval completedTime;
    double executionTime;
    struct Job *next;
} Job;

const char* getStatusString(JobStatus status);

#endif