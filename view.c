#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stddef.h>
#include <unistd.h>
#include "utils.h"

int shm_connect(char * name_shm, char **buffer);
void print_view(int shm, sem_t * sem );
int main(int argc, char *argv[]) {
    char name_shm[MAX_NAME_SIZE];
    if (argc == 2) {    //caso lo toma de argumentos
        strncpy(name_shm, argv[1], MAX_NAME_SIZE - 1);
        name_shm[MAX_NAME_SIZE - 1] = '\0';
    } else {
        int bytes_read = read(STDIN_FILENO, name_shm, MAX_NAME_SIZE - 1);
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        name_shm[bytes_read] = '\0';
    }
    char *buf = NULL;
    int shm_fd = shm_connect(name_shm, &buf);
    sem_t *sem = sem_open(SEM_NAME, O_RDWR, S_IRUSR | S_IWUSR, 0);
    print_view(shm_fd, sem);
    return 0;
}

int shm_connect(char * name_shm, char **buf){
    int fd = shm_open(name_shm, O_RDONLY, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }
    *buf = mmap(NULL, BUFFER_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    return fd;
}

void print_view(int shm, sem_t * sem ){
    char readBuf[BUFFER_SIZE] = "";
    char printBuf[BUFFER_SIZE] = "";
    while(strcmp(printBuf, SHM_FINISHED)!=0){
        sem_wait(sem);
        int i;
        for(i=0; read(shm, readBuf, 1) > 0 && *readBuf!= '\n'; i++){
            printBuf[i] = readBuf[0];
        }
        printBuf[i] = '\0';
        printf("%s\n", printBuf);
    }
}
