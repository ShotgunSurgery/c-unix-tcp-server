#include "../include/utils.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 1000

void str_add(int sockfd) {
  long arg1;
  long arg2;
  ssize_t n;
  char line[MAX_LINE];
  
  for (;;) {
    n = read(sockfd, line, MAX_LINE);
    if (n <= 0) {
      return;
    }
    
    line[n] = '\0';
    
    int no_of_args = sscanf(line, "%ld %ld", &arg1, &arg2);
    if (no_of_args == 2) {
      snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
    } else {
      snprintf(line, sizeof(line), "input error\n");
    }
    
    n = strlen(line);
    write(sockfd, line, n);
  }
}
