// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pipes.h"
#include "utils.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

void read_from_slaves(int files_qty,int slaves_qty, int slave_to_app_pipes[][2], FILE *results) ;
laves(int qty, int slave_to_app_pipes[][2], FILE *results);
void set_pipe_env(int qty, int app_to_slave_pipes[][2],
                  int slave_to_app_pipes[][2]);
void close_pipes(int app_to_slave_pipes[][2], int slave_to_app_pipes[][2],
                 int i, int qty);
void init_slaves(int qty, int pids[], int app_to_slave_pipes[][2],
                 int slave_to_app_pipes[][2]);
void init_pipes(int qty, int pipes[][2]);
void create_results(FILE **file);
void send_files_to_slaves(int files_qty, int slaves_qty,
                          int app_to_slave_pipes[][2], char const *argv[]);

int main(int argc, char const *argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "Error: No arguments provided.\n");
    exit(EXIT_FAILURE);
  }
  int files_qty = argc - 1;
  int slaves_qty = files_qty / 10 + 1;

  int slave_pids[slaves_qty];
  int app_to_slave_pipes[slaves_qty][2];
  int slave_to_app_pipes[slaves_qty][2];
  // init_pipes(slaves_qty, app_to_slave_pipes);
  // init_pipes(slaves_qty, slave_to_app_pipes);
  init_slaves(slaves_qty, slave_pids, app_to_slave_pipes, slave_to_app_pipes);
  FILE *results = NULL;
  create_results(&results);
  send_files_to_slaves(files_qty, slaves_qty, app_to_slave_pipes, argv);
  read_from_slaves(files_qty, slaves_qty, slave_to_app_pipes, results);


  // for (int i = 0; i < slaves_qty ; i++) {
  //   char buf[1024];
  //   read(slave_to_app_pipes[i][FD_READ], buf, 1024 - 1);
  //   puts(buf);
  // }

  // for(int i = 0; i< slaves_qty;i++){
  //     if(waitpid(slave_pids[i], NULL, 0)==-1){
  //         perror("waitpid");
  //         exit(EXIT_FAILURE);
  //     }
  // }

  return 0;
}

void read_from_slaves(int files_qty,int slaves_qty, int slave_to_app_pipes[][2], FILE *results) {
    int retval;
    fd_set rfds;
    size_t recieved = 0;
    while (recieved < files_qty) {
      int max_fd = 0;
      FD_ZERO(&rfds);
      for (int i = 0; i < slaves_qty; i++) {
        FD_SET(slave_to_app_pipes[i][FD_READ], &rfds);
        if (slave_to_app_pipes[i][FD_READ] > max_fd) {
          max_fd = slave_to_app_pipes[i][FD_READ];
        }
      }
      if (select(max_fd + 1, &rfds, NULL, NULL, NULL) == -1) {
          perror("select");
      }
      char buf[1024];
      char md5[1024];
      for (int i = 0; i < slaves_qty; i++) {
          if (FD_ISSET(slave_to_app_pipes[i][FD_READ], &rfds)) {
              int read_bytes;
              if ((read_bytes = read(slave_to_app_pipes[i][FD_READ], buf, 1024 - 1)) == -1) {
                  perror("read");
              }
              buf[read_bytes] = '\0';
              puts(buf);
              recieved++;
          }
      }
    }
}

void send_files_to_slaves(int files_qty, int slaves_qty,
                          int app_to_slave_pipes[][2], char const *argv[]) {
  int n = files_qty < slaves_qty ? files_qty : slaves_qty;

  for (int i = 0; i < files_qty; i++) {
    int slave = i % slaves_qty;

    printf("Sending file %s to slave %d\n", argv[i + 1], slave);

    // Write the file path to the pipe for the slave
    if (write(app_to_slave_pipes[slave][FD_WRITE], argv[i + 1], strlen(argv[i + 1])) == -1) {
      perror("Failed to send paths to slave process");
    }

    // Write a newline character after each file path
    if (write(app_to_slave_pipes[slave][FD_WRITE], "\n", 1) == -1) {
      perror("Failed to send newline to slave process");
    }
  }
}

void create_results(FILE **file) {
  *file = fopen("result.txt", "a");
  if (file == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  fprintf(*file, "N° -- Slave PID -- MD5 -- Filename\n");
  return;
}

void init_pipes(int qty, int pipes[][2]) {
  for (int i = 0; i < qty; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }
}

void init_slaves(int slaveQty, int pids[], int app_to_slave_pipes[][2],
                 int slave_to_app_pipes[][2]) {

  for (int i = 0; i < slaveQty; i++) {
    if (pipe(app_to_slave_pipes[i]) != 0 || pipe(slave_to_app_pipes[i]) != 0) {
      perror("Failed to create pipes");
    }
    pids[i] = fork();
    if (pids[i] == 0) {
      close(app_to_slave_pipes[i][FD_WRITE]);
      dup2(app_to_slave_pipes[i][FD_READ], STDIN_FILENO);

      close(slave_to_app_pipes[i][FD_READ]);
      dup2(slave_to_app_pipes[i][FD_WRITE], STDOUT_FILENO);

      execv("slave", (char *[]){"./slave", NULL});
    } else if (pids[i] > 0) {
      close(slave_to_app_pipes[i][FD_WRITE]);
      close(app_to_slave_pipes[i][FD_READ]);
    } else {
      perror("Error in fork");
    }
  }
}

// void init_slaves(int qty, int pids[],int app_to_slave_pipes[][2], int
// slave_to_app_pipes[][2]){

//     const char *args[] = {NULL};
//     for ( int i = 0; i < qty; i++)
//     {
//         int pid = fork();
//         if(pid < 0){
//             perror("fork");
//             exit(EXIT_FAILURE);
//         }
//         if(pid == 0){
//             close_pipes(app_to_slave_pipes,slave_to_app_pipes, i, qty);
//             set_pipe_env(qty,app_to_slave_pipes,slave_to_app_pipes);
//             execv("./slave", (char *const *)args);
//         }else{
//             pids[i] = pid;
//         }
//     }

//     for(int i=0; i < qty; i++){
//         close(app_to_slave_pipes[i][FILEDESC_READ]);
//         close(slave_to_app_pipes[i][FILEDESC_WRITE]);
//     }
//     return;
// }

void close_pipes(int app_to_slave_pipes[][2], int slave_to_app_pipes[][2],
                 int i, int qty) {
  for (int j = 0; j < qty; j++) {
    if (j != i) {
      close(app_to_slave_pipes[j][FILEDESC_READ]);
      close(app_to_slave_pipes[j][FILEDESC_WRITE]);
      close(slave_to_app_pipes[j][FILEDESC_READ]);
      close(slave_to_app_pipes[j][FILEDESC_WRITE]);
    }
  }
  close(app_to_slave_pipes[i][FILEDESC_WRITE]);
  close(slave_to_app_pipes[i][FILEDESC_READ]);
}
