#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>



int main(int argc, char const *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Error: No arguments provided.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
    {
        int fd[2];
        if (pipe(fd) == -1)
        {
            perror("pipe");
            return -1;
        }
        
        int pid = fork();


        if(pid < 0 ){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if(pid == 0){ //codigo hijo 
            char* args [] = {argv[i], NULL};
            execv("./slave", args);
            perror("execve");
        }
        else
        { // codigo padre

            waitpid(pid, NULL, 0);
            puts("done");
        }
    }


    // char buf[200];

    // FILE * fp = popen("md5sum carpeta/file", "r");
    // fgets(buf,200,fp);
    // buf[strlen(buf)] = '\0';
    // pclose(fp);
    // puts(buf);
    return 0;
}
