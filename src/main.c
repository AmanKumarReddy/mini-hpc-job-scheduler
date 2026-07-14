#include <stdio.h>
#include "scheduler.h"
#include <string.h>

int main(){
    Queue waitingQueue;
    Queue completedQueue;
    initializeQueue(&waitingQueue);
    initializeQueue(&completedQueue);

    int choice;
    int JobID = 1;
    char jobName[100];
    char command[200];
    Job *newJob;

    while(1){
        printf("\n============================================================\n");
        printf("     MINI HPC JOB SCHEDULER\n");
        printf("=============================================================\n");
        printf("1. Submit Job\n");
        printf("2. View Waiting\n");
        printf("3. Run Next Job\n");
        printf("4. View Completed\n");
        printf("5. Exit\n");
        printf("Enter Choice: ");
        scanf("%d",&choice);
        switch(choice){

            case 1:
            printf("\n Enter Job name: ");
            getchar();
            fgets(jobName , sizeof(jobName), stdin);
            jobName[strcspn(jobName, "\n")] = '\0';

            printf("Enter Command: ");
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = '\0';

            newJob = createJob(JobID++,jobName ,command);
            if(newJob != NULL){
                enqueue(&waitingQueue, newJob);
                printf("\nJob Submitted Successfully!\n");}
                break;

            case 2:
            printf("==================Waiting Queue=================\n");
            displayQueue(&waitingQueue);
            break;

            case 3:
            Job *completedJob = dequeue(&waitingQueue);
            if(completedJob == NULL){
                printf("\n No Jobs in Waiting Queue!\n");
            }
            else{
                printf("\nExecutin Job....\n");
                executeJob(completedJob);
                enqueue(&completedQueue, completedJob);
                printf("Job Finished Successfully!\n");
            }
            break;

            case 4:
            printf("\n============== Completed Jobs ==================\n");
            displayQueue(&completedQueue);
            break;

            case 5:
            return 0;

            default:
            printf("Invalid Choice\n");
        }

    }
}