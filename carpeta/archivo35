#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"

int setup_pipe_write(int pipe_fd) {
    // Close stdin
    close(STDIN_FILENO);

    // Duplicate pipe write end to stdin
    if (dup2(pipe_fd, STDIN_FILENO) == -1) {
        perror("dup2");
        return -1;
    }

    // Close the original pipe file descriptor
    close(pipe_fd);

    return 0;
}

int setup_pipe_read(int pipe_fd) {
    // Close stdout
    close(STDOUT_FILENO);

    // Duplicate pipe read end to stdout
    if (dup2(pipe_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        return -1;
    }

    // Close the original pipe file descriptor
    close(pipe_fd);

    return 0;
}

int write_pipe(const char *buffer) {
    // Write to stdout (which is now the pipe)
    int result = write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
    if (result < 0) {
        perror("write");
        return -1;
    }
    return result;
}

int read_pipe(char *buffer, int max_size) {
    int i = 0;
    char current_char;

    while (i < max_size - 1) {
        int result = read(STDIN_FILENO, &current_char, 1);
        if (result < 0) {
            perror("read");
            return -1;
        }
        if (result == 0 || current_char == '\0') {
            break;
        }
        buffer[i++] = current_char;
    }

    buffer[i] = '\0';
    return i;
}
