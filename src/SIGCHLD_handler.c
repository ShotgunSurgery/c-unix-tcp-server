#include "../include/utils.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void SIGCHLD_handler(int signo) {
  int saved_errno = errno;
  pid_t pid;
  int stat;
  
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
    printf("child %d terminated\n", pid);
  }
  
  errno = saved_errno;
  return;
}
