// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "utils.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0);

  char *filename = NULL;
  size_t linecapp = 0;
  ssize_t read_bytes;

  // Loop for reading input from stdin
  while ((read_bytes = getline(&filename, &linecapp, stdin)) != -1) {
    // Remove the newline character at the end, if present
    if (filename[read_bytes - 1] == '\n') {
      filename[read_bytes - 1] = '\0';
    }

    // Prepare the command to execute md5sum
    char command[strlen(filename) + strlen("md5sum") + 2];
    sprintf(command, "md5sum %s", filename);

    // Open a pipe to run the command
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
      perror("popen");
      free(filename); // Ensure filename is freed in case of error
      exit(EXIT_FAILURE);
    }

    // Read the md5sum result
    char result[50];
    if (fscanf(fp, "%32s", result) != 1) {
      perror("fscanf");
      pclose(fp);
      free(filename); // Ensure filename is freed in case of error
      exit(EXIT_FAILURE);
    }

    // Close the pipe and print the result
    pclose(fp);
    printf("%d %s %s\n", getpid(), result, filename);
  }

  // Free the filename buffer at the end

  free(filename);
  exit(EXIT_SUCCESS);
}
