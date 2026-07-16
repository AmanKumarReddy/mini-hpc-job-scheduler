#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../common/ipc.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: cancel <job_id>\n");
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
    sprintf(message, "CANCEL|%d|%s", my_pid, argv[1]);
    write(fd, message, strlen(message));
    close(fd);

    int reply_fd = open(reply_fifo, O_RDONLY);
    if (reply_fd != -1) {
        char response[256];
        int bytes = read(reply_fd, response, sizeof(response) - 1);
        if (bytes > 0) {
            response[bytes] = '\0';
            printf("%s", response);
        }
        close(reply_fd);
    }

    unlink(reply_fifo);
    return 0;
}
