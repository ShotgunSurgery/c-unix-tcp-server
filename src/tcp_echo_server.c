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

  // read() reads data from a file descriptor into a buffer, returns number of bytes read, 0 if client closed connection, -1 if error
again:
  while ((n = read(sockdf, buffer, MAXLINE)) > 0) {
    write(sockdf, buffer, n); // write() is used to send data from buffer to socket it returns no of bytes written, -1 on error

    // printf("Client message: %s", buffer);	 this is wrong because read() does not null terminate string and %s expects a null terminated string so printf may print garbage etc ...
    printf("Client message: ");
    fwrite(
        buffer, 1, n,
        stdout); // second argument is size of one element so 1*n = n bytes (n is the number of elements), fwrite() copies raw bytes from memory into stdio output buffer (stdout), it doesnot garuntee printing
    fflush(stdout); // this forces stdout buffer to be physically written on terminal
                    // if(n < 0 && errno == EINTR)goto again;
                    // else if ( n < 0){
                    // 	perror("str_echo: read error");
                    // 	exit(1); // terminates calling process normally
                    // }
  }
  // if(n < 0 && errno != EINTR){
  if (n < 0) {
    perror("str_echo: read error");
    exit(1);
    // child terminates by calling exit
  }
  // the closing of child causes the FIN from server and ACK from the client and the connection is completely close, the client socket enters he TIME_WAIT state i.e. the connection is fully closed but the side is waiting befoure fully closing the socket, this wait is to ensure that any delayed or duplicate packets from the other side from the old connection are to be discarded and a final ACK message to be sent, after this a SIGCHLD signal is sent to the parent and the child enters zombie state
}

int main(int argc, char **argv) {
  // create socket
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  // create a socket address for server and fill details for server
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr)); // we set add feils of struct to 0 to start from a valid known state, if there is garbage values in them then it can cause undefined behaviour
  server_addr.sin_addr.s_addr = htonl(
      INADDR_ANY); // the struct sockaddr_in contains the member sin_addr which is of the type in_addr and whoose sole purpose is to store a IPv4 address and that struct contains only one feild i.e. s_addr
  // server_addr.sin_port = htonl(0);
  server_addr.sin_port = htons(8080);
  server_addr.sin_family = AF_INET;

  // bind the socket to the address
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
    printf("Server bound to port 8080\n");
  } else {
    printf("Failed to bind server\n");
  }
  // mark the TCP bound socket as passive
  if (listen(listen_fd, LISTENQ) == 0) // maximum number of pending connections the kernel is allowed to queue for a listening TCP socket
  {
    printf("Server listning on port 8080\n");
  }
  // --LEGACY---
  // create a socket to connect with client
  // int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  // --LEGACY---

  // create a socket address to store the client infomation
  struct sockaddr_in client_addr;

  pid_t childPid;
  int connection_fd;
  socklen_t client_len;
  Signal(SIGCHLD, SIGCHLD_handler);
  // signal(SIGCHLD, SIG_DFL); // handle signals (inbuilt handler)

  // keep the server running
  while (1) {
    // accept clients connection
    // the client calls connect and the server calls accept - causes TCP handshake - afterwards both functions return to server and client respectively
    // for (;;) {
    // } infinite loop
    client_len = sizeof(client_addr);
    connection_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (connection_fd < 0) {
      if (errno == EINTR ||
          errno ==
              ECONNABORTED) { // EINTR means intrupted by a signal, ECONNABORTED happens when 3 way handsake is done and the connection is put into the servers completed connection queue but befoure the server calls accept() the client sends RST(reset) aborting the connection hence the accept returns -1 i.e. connection_fd < 0 and the errno is set to ECONNABORTED
        continue;
      } else {
        perror("accept error");
        continue;
      }
    }

    printf("Client Connected\n");
    // over here when the fork returns 0 to the child and the pid to the parent so the child evaluates the if condition to true and enters inside the if scope but the parent recieves the childs pid and hence evaluates the condition to false and skips the block entirely

    // saving space and being direct below is what caused the error to silently go unoticed so instead USE BRACKETS
    // if((childPid = fork()) == 0){ -- BAD ONE
    childPid = fork();
    if (childPid == 0) {
      close(listen_fd); // after the parent creates a child process it inherits all the open descriptors which include the listen_fd as well but it has got no use for it hence it closes it
      printf("ChildPid: %d\n", (int)getpid());
      str_echo(connection_fd);
      exit(0);
    }
    close(connection_fd);
  }
}

// --- FOOTNOTES ---
// if the parent process is terminated and that process has children in zombie state (zombie state's purpose is to maintain info about the child for the parent to fetch at some time later like process id, termination status, info on resource utilization etc...), these zombies take up kernel space and we will eventually run out of processess so that's why we must clean them up, when the parent process terminates while it has children in zombie state the init process inherits them assigns everyone id 1 and waits for them

// slow sys call - call that can block forever. ex: accept(), read()
