// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include "pipes.h"
#include <sys/select.h>

//app

void init_slaves(int qty, int pids[]);
void init_pipes(int qty, int pipes[][2]);
void init_slave_to_app_pipes(int qty, int pipes[][2]);
void create_results(FILE ** file);
void send_files_to_slaves(int files_qty, int slaves_qty, int app_to_slave_pipes[][2], char const *argv[]);
void read_from_slaves(int slaves_qty, int slave_to_app_pipes[][2], FILE * results);

int main(int argc, char const *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Error: No arguments provided.\n");
        exit(EXIT_FAILURE);
    }
    int files_qty = argc - 1;
    int slaves_qty = files_qty/10 + 1;
    printf("cantidad de archivos: %d\n", files_qty);
    printf("cantidad de argumentos: %d\n", argc);
    printf("cantidad de esclavos: %d\n", slaves_qty);
    int slave_pids[slaves_qty];
    init_slaves(slaves_qty, slave_pids);
    int app_to_slave_pipes[slaves_qty][2];
    int slave_to_app_pipes[slaves_qty][2];
    init_pipes(slaves_qty, app_to_slave_pipes);
    init_pipes(slaves_qty, slave_to_app_pipes);
    FILE * results = fopen("result.txt", "a");
    if(results == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(results, "N° -- Slave PID -- MD5 -- Filename\n");
    send_files_to_slaves(files_qty, slaves_qty, app_to_slave_pipes, argv);


>>>>>>> 8ac3d0e (Revert "Refactor: Replace magic number 200 with BUFFER_SIZE constant for better readability and maintainability")
    for(int i = 0; i< slaves_qty;i++){
        if(waitpid(slave_pids[i], NULL, 0)==-1){
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }

    // for (int i = 1; i < argc; i++)
    // {
    //     int fd[2];
    //     if (pipe(fd) == -1)
    //     {
    //         perror("pipe");
    //         return -1;
    //     }

    //     int pid = fork();


    //     if(pid < 0 ){
    //         perror("fork");
    //         exit(EXIT_FAILURE);
    //     }

    //     if (pid == 0)
    //     {
    //         puts(argv[i]);
    //         const char *args[] = {argv[i], NULL};
    //         execv("./slave", (char *const *)args);
    //         perror("execv");
    //     }

    //     else
    //     { // codigo padre

    //         waitpid(pid, NULL, 0);
    //         puts("done");
    //     }
    // }


    // char buf[200];

    // FILE * fp = popen("md5sum carpeta/file", "r");
    // fgets(buf,200,fp);
    // buf[strlen(buf)] = '\0';
    // pclose(fp);
    // puts(buf);
    return 0;
}

void read_from_slaves(int slaves_qty, int slave_to_app_pipes[][2], FILE *results) {
    fd_set readfds;
    int max_fd = -1;
    char buffer[BUFFER_SIZE * 2];
    int active_slaves = slaves_qty;

    while (active_slaves > 0) {
        FD_ZERO(&readfds);
        max_fd = -1;

        for (int i = 0; i < slaves_qty; i++) {
            if (slave_to_app_pipes[i][0] != -1) {
                FD_SET(slave_to_app_pipes[i][0], &readfds);
                if (slave_to_app_pipes[i][0] > max_fd) {
                    max_fd = slave_to_app_pipes[i][0];
                }
            }
        }

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < slaves_qty; i++) {
            if (slave_to_app_pipes[i][0] != -1 && FD_ISSET(slave_to_app_pipes[i][0], &readfds)) {
                ssize_t bytes_read = read(slave_to_app_pipes[i][0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    fprintf(results, "%s", buffer);
                    fflush(results);
                } else if (bytes_read == 0) {
                    close(slave_to_app_pipes[i][0]);
                    slave_to_app_pipes[i][0] = -1;
                    active_slaves--;
                } else {
                    perror("read");
                }
            }
        }
    }
}

void send_files_to_slaves(int files_qty, int slaves_qty, int app_to_slave_pipes[][2], char const *argv[]) {
    for (int i = 1; i <= files_qty; i++) {
        int slave = (i - 1) % slaves_qty;
        write_pipe(argv[i]);
    }

    // Close write ends of pipes to signal end of input
    for (int i = 0; i < slaves_qty; i++) {
        close(app_to_slave_pipes[i][1]);
    }
}

void create_results(FILE ** file){

    return;
}


void init_pipes(int qty, int pipes[][2]){
    for (int i = 0; i < qty; i++)
    {
        if(pipe(pipes[i]) == -1){
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
}



void init_slaves(int qty, int pids[]){

    const char *args[] = {NULL};
    for ( int i = 0; i < qty; i++)
    {
        int pid = fork();
        if(pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if(pid == 0){
            execv("./slave", (char *const *)args);
        }else{
            pids[i] = pid;
        }
    }

}
