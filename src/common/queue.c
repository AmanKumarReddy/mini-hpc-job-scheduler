#include <stdio.h>
#include "queue.h"

void initializeQueue(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

void enqueue(Queue *q, Job *job) {
    job->next = NULL;
    if (q->rear == NULL) {
        q->front = job;
        q->rear = job;
    } else {
        q->rear->next = job;
        q->rear = job;
    }
}

Job* dequeue(Queue *q) {
    if (q->front == NULL) {
        return NULL;
    }
    Job *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    return temp;
}
