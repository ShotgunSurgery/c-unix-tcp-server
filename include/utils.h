#include <stdio.h>
#ifndef UTILS_H
#define UTILS_H

typedef void Sigfunc(int);

Sigfunc *Signal(int signal_no, Sigfunc *function_ptr);
void SIGCHLD_handler(int signo);
void str_add(int sockfd);
void str_cli(FILE *file_ptr, int sockfd);

#endif
