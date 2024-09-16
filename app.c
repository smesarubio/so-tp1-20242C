// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pipes.h"
#include "utils.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

void close_pipes(int qty, int app_to_slave_pipes[][2], int slave_to_app_pipes[][2]);
void manage_dynamic_file_sending(int files_qty, int slaves_qty,
                                 int app_to_slave_pipes[][2],
                                 int slave_to_app_pipes[][2],
                                 char const *argv[], FILE *results, sem_t *sem,
                                 int sm_fd);
void send_initial_files(int files_qty, int slaves_qty,
                        int app_to_slave_pipes[][2], char const *argv[]);
void init_slaves(int qty, int pids[], int app_to_slave_pipes[][2],
                 int slave_to_app_pipes[][2]);
void create_results(FILE **file);
int shm_init(char **map_result);
int wait_view();

int view_on = 0;
int main(int argc, char const *argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "Error: No arguments provided.\n");
    exit(EXIT_FAILURE);
  }
  sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  view_on = wait_view();
  char *map_result;
  int shm_fd;
  if(view_on == 1){
    shm_fd = shm_init(&map_result);
  }

  int files_qty = argc - 1;
  int slaves_qty = files_qty / 10 + 1;
  int slave_pids[slaves_qty];
  int app_to_slave_pipes[slaves_qty][2];
  int slave_to_app_pipes[slaves_qty][2];
  init_slaves(slaves_qty, slave_pids, app_to_slave_pipes, slave_to_app_pipes);
  FILE *results = NULL;
  create_results(&results);
  send_initial_files(files_qty, slaves_qty, app_to_slave_pipes, argv);
  manage_dynamic_file_sending(files_qty, slaves_qty, app_to_slave_pipes,
                              slave_to_app_pipes, argv, results, sem, shm_fd);

  close_pipes(slaves_qty, app_to_slave_pipes, slave_to_app_pipes);
  return 0;
}


void close_pipes(int qty, int app_to_slave_pipes[][2], int slave_to_app_pipes[][2]) {
  for (int i = 0; i < qty; i++) {
    close(app_to_slave_pipes[i][FD_READ]);
    close(slave_to_app_pipes[i][FD_WRITE]);
  }
}

int wait_view() {
  sleep(2); // yo le pongo 8 porque no llego a correr el proceso view
  char buffer[128];
  char process[10] = "view";
  FILE *pipe = popen("ps -e | grep view", "r");

  if (pipe == NULL) {
    perror("popen");
    return -1;
  }

  int process_found = 0;

  // Lee la salida del comando
  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    if (strstr(buffer, process) != NULL) {
      process_found = 1;
    }
  }
  pclose(pipe);
  return process_found;
}

void send_initial_files(int files_qty, int slaves_qty,
                        int app_to_slave_pipes[][2], char const *argv[]) {
  // Enviar un archivo a cada esclavo al inicio
  for (int i = 0; i < slaves_qty && i < files_qty; i++) {
    // printf("Sending file %s to slave %d\n", argv[i + 1], i);
    if (write(app_to_slave_pipes[i][FD_WRITE], argv[i + 1],
              strlen(argv[i + 1])) == -1) {
      perror("write");
    }
    if (write(app_to_slave_pipes[i][FD_WRITE], "\n", 1) == -1) {
      perror("write");
    }
  }
}

void manage_dynamic_file_sending(int files_qty, int slaves_qty,
                                 int app_to_slave_pipes[][2],
                                 int slave_to_app_pipes[][2],
                                 char const *argv[], FILE *results, sem_t *sem,
                                 int shm_fd) {
  fd_set rfds;
  int files_sent = slaves_qty; // Ya se enviaron los archivos iniciales
  int files_processed = 0;

  while (files_processed < files_qty) {
    FD_ZERO(&rfds);
    int max_fd = 0;

    // Monitorear los pipes de lectura desde los esclavos
    for (int i = 0; i < slaves_qty; i++) {
      FD_SET(slave_to_app_pipes[i][FD_READ], &rfds);
      if (slave_to_app_pipes[i][FD_READ] > max_fd) {
        max_fd = slave_to_app_pipes[i][FD_READ];
      }
    }

    if (select(max_fd + 1, &rfds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    char buf[1024];
    for (int i = 0; i < slaves_qty; i++) {
      if (FD_ISSET(slave_to_app_pipes[i][FD_READ], &rfds)) {
        int read_bytes = read(slave_to_app_pipes[i][FD_READ], buf, 1024 - 1);
        if (read_bytes > 0) {
          buf[read_bytes] = '\0';
          if(view_on){
            write(shm_fd, buf, strlen(buf));
          }
          // Escribir el resultado en el archivo
          fprintf(results, "%d %s", files_processed, buf);
          files_processed++;
          sem_post(sem);
          // Si aún quedan archivos por enviar
          if (files_sent < files_qty) {

            // Enviar un nuevo archivo al esclavo que acaba de liberar
            if (write(app_to_slave_pipes[i][FD_WRITE], argv[files_sent + 1],
                      strlen(argv[files_sent + 1])) == -1) {
              perror("Failed to send path to slave process");
            }
            if (write(app_to_slave_pipes[i][FD_WRITE], "\n", 1) == -1) {
              perror("Failed to send newline to slave process");
            }
            files_sent++;
          }
        }
      }
    }
  }

  char end[sizeof(SHM_FINISHED)] = SHM_FINISHED;
  write(shm_fd, end, sizeof(SHM_FINISHED));
  sem_post(sem);
}

void create_results(FILE **file) {
  *file = fopen("result.txt", "w");
  if (file == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  fprintf(*file, "N° -- Slave PID -- MD5 -- Filename\n");
  return;
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

int shm_init(char **map_result) {
  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(fd, SHM_SIZE) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  *map_result = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (*map_result == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  printf("%s", SHM_NAME);
  fflush(stdout);
  return fd;
}
