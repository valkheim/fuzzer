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
# define CLS() (printf("\033[1;1H\033[2J"))
#else
# define CLS() IDLE
#endif

int ferr(char *label, char *file, int line);
void ferrx(char *label, char *file, int line);
