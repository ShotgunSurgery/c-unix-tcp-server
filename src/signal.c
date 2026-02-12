#define _POSIX_C_SOURCE 200809L
#include "../include/utils.h"
#include <signal.h>

typedef void Sigfunc(int);

Sigfunc *Signal(int signal_no, Sigfunc *function_ptr) {
  struct sigaction new_signal_to_install, old_signal;
  new_signal_to_install.sa_handler = function_ptr;
  sigemptyset(&new_signal_to_install.sa_mask);
  new_signal_to_install.sa_flags = 0;
  
  if (signal_no == SIGALRM) {
#ifdef SA_INTERRUPT
    new_signal_to_install.sa_flags |= SA_INTERRUPT;
#endif
  } else {
#ifdef SA_RESTART
    new_signal_to_install.sa_flags |= SA_RESTART;
#endif
  }
  
  if (sigaction(signal_no, &new_signal_to_install, &old_signal) < 0) {
    return (SIG_ERR);
  }
  return (old_signal.sa_handler);
}
