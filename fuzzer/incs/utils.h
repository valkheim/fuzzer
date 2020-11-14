#pragma once

#define IDLE do {} while (0);

#ifdef DEBUG
# include <stdio.h>
# define LOG(fmt, ...) do { \
   fprintf(stderr, "%s:%d:%s: " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); \
 } while (0);
#else
# define LOG(fmt, ...) IDLE
#endif

#ifndef DEBUG
# define CLS() (printf("\e[1;1H\e[2J"))
#else
# define CLS() IDLE
#endif

static char * const sys_siglist[] = {
  "",
  "sighup",
  "sigint",
  "sigquit",
  "sigill",
  "sigtrap",
  "sigabrt",
  "sigemt",
  "sigfpe",
  "sigkill",
  "sigbus",
  "sigsegv",
  "sigsys",
  "sigpipe",
  "sigalrm",
  "sigterm",
  "sigurg",
  "sigstop",
  "sigtstp",
  "sigcont",
  "sigchld",
  "sigttin",
  "sigttou",
  "sigio",
  "sigxcpu",
  "sigxfsz",
  "sigvtalrm",
  "sigprof",
  "sigwinch",
  "siginfo",
  "sigusr1",
  "sigusr2",
};

int ferr(char *label, char *file, int line);
void ferrx(char *label, char *file, int line);
