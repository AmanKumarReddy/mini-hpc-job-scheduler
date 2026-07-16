#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../common/queue.h"

void startScheduler(Queue *waitingQueue, Queue *completedQueue);
void executeJob(Job *job);
void logJob(Job *job);
void displayAdminStatus(int clientPID, Queue *waitingQueue, Queue *completedQueue);
void processJob(int clientPID, int jobID, char command[], char workingDirectory[], Queue *waitingQueue, Queue *completedQueue);
void processStatus(int clientPID, int jobID, Queue *waitingQueue, Queue *completedQueue);
void processCancel(int clientPID, int jobID, Queue *waitingQueue, Queue *completedQueue);
void processOutput(int clientPID, int jobID, Queue *completedQueue);
void cleanupExpiredJobs(Queue *completedQueue);

#endif