#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXLINE 1000

void str_cli(FILE *fp, int sockfd) {
  char sendline[MAXLINE], recvline[MAXLINE];

  while (fgets(sendline, MAXLINE, fp) != NULL) {
    write(sockfd, sendline, strlen(sendline));

    ssize_t n = read(sockfd, recvline, MAXLINE);
    if (n <= 0) {
      perror("str_cli: server terminated prematurely");
      return;
    }

    printf("Server replied: ");
    fwrite(recvline, 1, n, stdout);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <server-ip>\n", argv[0]);
    exit(1);
  }

  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    perror("socket creation failed");
    exit(1);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_port = htons(8080);
  server_addr.sin_family = AF_INET;

  if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
    perror("invalid address or address not supported");
    close(client_fd);
    exit(1);
  }

  if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("connection failed");
    close(client_fd);
    exit(1);
  }

  str_cli(stdin, client_fd);

  close(client_fd);
  exit(0);
}
