#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pipes.h"
#include "utils.h"


int main(int argc, char const *argv[])
{
    char filename[BUFFER_SIZE];
    char result[BUFFER_SIZE];
    int file_count = 0;

    while (read_pipe(STDIN_FILENO, filename) > 0)
    {
        fprintf(stderr, "Slave %d received filename: %s\n", getpid(), filename);

        if (strcmp(filename, "EXIT") == 0) {
            break;  // Exit signal received
        }

        char command[BUFFER_SIZE];
        snprintf(command, sizeof(command), "md5sum \"%s\"", filename);

        FILE *fp = popen(command, "r");
        if (fp == NULL) {
            snprintf(result, sizeof(result), "%d Error executing md5sum for %s\n", getpid(), filename);
        } else {
            if (fgets(result, sizeof(result), fp) != NULL) {
                result[strcspn(result, "\n")] = 0;
            } else {
                snprintf(result, sizeof(result), "%d Error reading md5sum for %s\n", getpid(), filename);
            }
            pclose(fp);
        }

        char output[BUFFER_SIZE * 2];
        snprintf(output, sizeof(output), "%d -- %d -- %s\n", ++file_count, getpid(), result);

        if (write_pipe(STDOUT_FILENO, output) < 0) {
            fprintf(stderr, "Error writing to pipe\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
