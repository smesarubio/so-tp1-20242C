// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pipes.h"
#include "utils.h"


int main(int argc, char const *argv[])
{
    int ready = 1;
    char filename[BUFFER_SIZE];
    while (ready>0)
    {
        ready = read_pipe(STDIN_FILENO, filename);
        char buf[BUFFER_SIZE];
        char command[BUFFER_SIZE];
        char result[BUFFER_SIZE];
        sprintf(command, "md5sum %s", filename);
        FILE *fp = popen(command, "r");
        if(fp == NULL){
            perror("popen");
            exit(EXIT_FAILURE);
        }

        fgets(buf, BUFFER_SIZE, fp);
        buf[strlen(buf) + 1] = '\0';
        pclose(fp);
        sprintf(result, "%d %s", getpid(), buf);
        write_pipe(1, result);

    }

    close(STDOUT_FILENO);

    exit(EXIT_SUCCESS);
    return 0;
}
