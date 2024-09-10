#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"


int read_pipe(int fd, char *buffer) {
    char  current_char[1] = {1};
    int i;

    for (i = 0; i < MAX_PATH && current_char[0] != 0; i++) {
        int result = read(fd, current_char, 1);
        if (result == -1) {
            perror("read");
            return -1;
        }
        buffer[i] = current_char[0];
    }
    buffer[i] = 0;
    return i;
}


int write_pipe(int fd, const char *buffer) {
    int len = strlen(buffer);
    int result = write(fd, buffer, len);
    if (result < 0) {
        return -1;  // Error
    }
    if (write(fd, "\0", 1) != 1) {  // Write null terminator
        return -1;
    }
    return result + 1;

    return result;
}
