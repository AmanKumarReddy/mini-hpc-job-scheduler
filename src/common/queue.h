#ifndef QUEUE_H
#define QUEUE_H

#include "job.h"

typedef struct {
    Job *front;
    Job *rear;
} Queue;

void initializeQueue(Queue *q);
void enqueue(Queue *q, Job *job);
Job* dequeue(Queue *q);

#endif