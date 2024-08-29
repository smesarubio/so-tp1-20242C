#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

int write_pipe(int fd, const char *buffer) {
    int result = write(fd, buffer, strlen(buffer)+1);
    if (result < 0) {
        return -1;  // Error
    }
    return result;
}


int read_pipe(int fd, char *buffer) {
    char  current_char[] = {1};
    int result;
    int i;
    for (i = 0; i<MAX_PATH && current_char[0]!=0 ; i++) {
        result = read(fd, current_char, 1);
        if (result < 0) {
            return -1;  // Error
        }
        buffer[i] = current_char[0];
    }
    buffer[i] = 0;
    return result;
}
