#include "../include/utils.h"
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTENQ 128
#define MAXLINE 1024

void str_echo(int sockdf) {
  ssize_t n;
  char buffer[MAXLINE];

  while ((n = read(sockdf, buffer, MAXLINE)) > 0) {
    write(sockdf, buffer, n);
    printf("Client message: ");
    fwrite(buffer, 1, n, stdout);
    fflush(stdout);
  }

  if (n < 0) {
    perror("str_echo: read error");
    exit(1);
  }
}

int main(int argc, char **argv) {
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    perror("socket creation failed");
    exit(1);
  }

  int reuse = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(1);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(8080);
  server_addr.sin_family = AF_INET;

  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    close(listen_fd);
    exit(1);
  }
  printf("Server bound to port 8080\n");

  if (listen(listen_fd, LISTENQ) < 0) {
    perror("listen failed");
    close(listen_fd);
    exit(1);
  }
  printf("Server listening on port 8080\n");

  struct sockaddr_in client_addr;
  pid_t childPid;
  int connection_fd;
  socklen_t client_len;

  Signal(SIGCHLD, SIGCHLD_handler);

  while (1) {
    client_len = sizeof(client_addr);
    connection_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (connection_fd < 0) {
      if (errno == EINTR || errno == ECONNABORTED) {
        continue;
      } else {
        perror("accept error");
        continue;
      }
    }

    printf("Client Connected\n");

    childPid = fork();
    if (childPid < 0) {
      perror("fork failed");
      close(connection_fd);
      continue;
    }
    if (childPid == 0) {
      close(listen_fd);
      printf("ChildPid: %d\n", (int)getpid());
      // str_echo(connection_fd);
      str_add(connection_fd);
      close(connection_fd);
      exit(0);
    }
    close(connection_fd);
  }
}
