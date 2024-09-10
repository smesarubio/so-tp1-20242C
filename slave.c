// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
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

    setvbuf(stdout, NULL, _IONBF, 0);

    char *filename;
    size_t linecapp = 0;
    int read_bytes = 1;

    while ((read_bytes = getline(&filename, &linecapp, stdin)) >=0) {
        if(filename[read_bytes-1]=='\n'){
            filename[read_bytes-1] = 0;
        }
        char command[strlen(filename) + strlen("md5sum") + 2];
        sprintf(command, "md5sum %s", filename);

        FILE *fp = popen(command, "r");
        if(fp == NULL){
            perror("popen");
            exit(EXIT_FAILURE);
        }
        char result[50];
        if(fscanf(fp, "%32s", result)!=1){
            perror("fprintf");
            exit(EXIT_FAILURE);
        }
        pclose(fp);
        printf("%d %s %s\n", getpid(), result, filename);
    }
    free(filename);
    exit(EXIT_SUCCESS);
    return 0;
}
