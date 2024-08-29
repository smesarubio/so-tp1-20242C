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
    while (1)
    {
        /* code */
    }

    char buf[200];
    char command[200];
    sprintf(command, "md5sum %s", argv[0]);
    FILE *fp = popen(command, "r");
    fgets(buf, 200, fp);
    pclose(fp);
    FILE * file = fopen("result.txt", "a"); //mal debe de volver a app.
    fprintf(file, buf);
    fclose(file);
    return 0;
}
