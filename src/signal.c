#define _POSIX_C_SOURCE 200809L
#include "../include/utils.h"
#include <signal.h>

typedef void Sigfunc(int);

Sigfunc *Signal(int signal_no, Sigfunc *function_ptr) {
  struct sigaction new_signal_to_install, old_signal;
  new_signal_to_install.sa_handler = function_ptr;
  sigemptyset(&new_signal_to_install.sa_mask); // we set the sa_mask member to empty set which means that no additional signals wil be blocked while our signal handler is running
  new_signal_to_install.sa_flags = 0; // sa_flags is a bitmask feild, basically signals can arrive in between syscalls so in order to tell the kernel what to do in such a situation we have this
  if (signal_no == SIGALRM) {
#ifdef SA_INTERRUPT
    new_signal_to_install.sa_flags |= SA_INTERRUPT;
#endif
  } else {
#ifdef SA_RESTART // SA_RESTART is a optional flag, if a system call is interrupted by this signal it will be automatically restarted by the kernel
    new_signal_to_install.sa_flags |= SA_RESTART;
#endif
  }
  if (sigaction(signal_no, &new_signal_to_install, &old_signal) < 0) { // sigaction() returns 0 on successful completion, -1 on
    return (SIG_ERR);                                                  // SIG_ERR indicates error while call to signal function
  }
  return (old_signal.sa_handler);
}

// ---ADDITIONAL POINTS---
//if i recieve say a SIG_INT signal then the signal handler will call it's handler and while the handler is executing the kernel will block the SIG_INT
//if a signal is generated one or more times while it is blocked it is normally delivered only one time after the signal is unblocked, UNIX singals are not qued
//using the sigprocmask function we can protect a critical region of code by preventing certain signals from being caught while that region of code is running
