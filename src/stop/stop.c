#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "../common/ipc.h"

int main() {
    if (getuid() != 0) {
        fprintf(stderr, "Error: Only the administrator (root/sudo) can stop the HPC scheduler.\n");
        return 1;
    }

    int fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Error: Scheduler daemon is not running.\n");
        return 1;
    }
    
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    char message[64];
    sprintf(message, "ADMIN_STOP|%d|%d", getpid(), getuid());
    write(fd, message, strlen(message));
    close(fd);
    
    printf("Shutdown signal sent to HPC Scheduler.\n");
    return 0;
}