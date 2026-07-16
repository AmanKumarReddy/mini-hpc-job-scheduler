#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../common/ipc.h"

int main(int argc, char *argv[]) {
    if (argc > 2) {
        printf("Usage:\n  status          (Admin: Global Summary)\n  status <job_id> (User: Specific Job details)\n");
        return 1;
    }

    if (argc == 1 && getuid() != 0) {
        fprintf(stderr, "Error: Global status summary is restricted to the administrator.\n");
        return 1;
    }

    pid_t my_pid = getpid();
    char reply_fifo[64];
    sprintf(reply_fifo, "/tmp/hpc_reply_%d", my_pid);

    if (mkfifo(reply_fifo, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }

    int fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Error: Scheduler daemon is not running.\n");
        unlink(reply_fifo);
        return 1;
    }

    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    char message[128];
    if (argc == 1) {
        sprintf(message, "ADMIN_STATUS|%d|%d", my_pid, getuid());
    } else {
        sprintf(message, "STATUS|%d|%s", my_pid, argv[1]);
    }
    
    write(fd, message, strlen(message));
    close(fd);

    int reply_fd = open(reply_fifo, O_RDONLY);
    if (reply_fd != -1) {
        char response[1024];
        int bytes;
        while ((bytes = read(reply_fd, response, sizeof(response) - 1)) > 0) {
            response[bytes] = '\0';
            printf("%s", response);
        }
        close(reply_fd);
    }
    usleep(10000);
    unlink(reply_fifo);
    return 0;
}