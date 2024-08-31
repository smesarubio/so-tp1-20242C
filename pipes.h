#ifndef PIPES_H
#define PIPES_H

// Function to set up the write end of a pipe
int setup_pipe_write(int pipe_fd);

// Function to set up the read end of a pipe
int setup_pipe_read(int pipe_fd);

// Function to write a string to a pipe
int write_pipe(const char *buffer);

// Function to read a string from a pipe
int read_pipe(char *buffer, int max_size);

#endif // PIPES_H
