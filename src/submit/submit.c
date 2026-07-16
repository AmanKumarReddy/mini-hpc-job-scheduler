#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../common/ipc.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: submit <command>\n");
        return 1;
    }

    char command[1024] = "";
    for (int i = 1; i < argc; i++) {
        if (strlen(command) + strlen(argv[i]) + 2 > sizeof(command)) {
            fprintf(stderr, "Error: Reconstructed command is too long.\n");
            return 1;
        }
        strcat(command, argv[i]);
        if (i < argc - 1) {
            strcat(command, " ");
        }
    }

    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
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

    char message[2048];
    snprintf(message, sizeof(message), "SUBMIT|%d|%s|%s", my_pid, cwd, command);
    write(fd, message, strlen(message));
    close(fd);

    int reply_fd = open(reply_fifo, O_RDONLY);
    if (reply_fd != -1) {
        char response[512];
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