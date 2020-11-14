#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "utils.h"

int ferr(char *label, char *file, int line)
{
  if (errno == 0)
    return errno;

  fprintf(stderr, "Error %d in %s:%d during ", errno, file, line);
  perror(label);
  return errno;
}

void ferrx(char *label, char *file, int line)
{
  exit(ferr(label, file, line));
}
