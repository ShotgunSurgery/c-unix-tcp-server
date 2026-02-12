#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXLINE 1000
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void str_cli(FILE *file_ptr, int socket_fd) {
  fd_set arr_fds;
  char buffer[MAXLINE];
  int stdin_EOF = 0;
  int max_file_descriptors = 0;
  int n;

  FD_ZERO(&arr_fds);
  for (;;) {
    if (stdin_EOF == 0) {
      FD_SET(fileno(file_ptr), &arr_fds);
      FD_SET(socket_fd, &arr_fds);
      max_file_descriptors = MAX(fileno(file_ptr), socket_fd) + 1;
      select(max_file_descriptors, &arr_fds, NULL, NULL, NULL);
      if (FD_ISSET(socket_fd, &arr_fds)) {
        if ((n = read(socket_fd, buffer, MAXLINE))) {
          if (stdin_EOF == 1) {
            return;
          } else {
            printf("str_cli: server terminated prematurly");
          }
          write(fileno(stdout), buffer, n);
        }
        if (FD_ISSET(fileno(file_ptr), &arr_fds)) {
          if ((n == read(fileno(file_ptr), buffer, MAXLINE) == 0)) {
            stdin_EOF = 1;
            shutdown(socket_fd, SHUT_WR);
            FD_CLR(socket_fd, &arr_fds);
            continue;
          }
          write(socket_fd, buffer, n);
        }
      }
    }
  }
}
