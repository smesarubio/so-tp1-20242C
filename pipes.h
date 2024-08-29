#ifndef PIPE_H
#define PIPE_H

int write_pipe(int fd, const char *buffer);
int read_pipe(int fd, char *buffer);

#endif // PIPE_H
