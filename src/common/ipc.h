#ifndef IPC_H
#define IPC_H

#define FIFO_PATH "/tmp/hpc_scheduler_fifo"

void createSchedulerFIFO(void);
int openSchedulerFIFO(void);

#endif