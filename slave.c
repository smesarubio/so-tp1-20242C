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
    char result[1024];
    char filename[200];
    char buf[200];
    char command[200];
    int ready = 1;
    while (ready>0) {

        ready = read_pipe(STDIN_FILENO, filename);
        if(ready==-1){
            perror("read");
            exit(EXIT_FAILURE);
        }
        sprintf(command, "md5sum %s", argv[0]);
        FILE *fp = popen(command, "r");
        if(fp == NULL){
            perror("popen");
            exit(EXIT_FAILURE);
        }
        fgets(buf, 200, fp);
        buf[strlen(buf)] = '\0';

        pclose(fp);

        sprintf(result, "%d %s", getpid(), buf);
        write_pipe(FILEDESC_WRITE, result);
    }

    close(STDOUT_FILENO);
    exit(EXIT_SUCCESS);
    return 0;
}
