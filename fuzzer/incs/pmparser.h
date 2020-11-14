#pragma once

#include <limits.h>

#define LINE_LEN (512)
#define LINES_LEN (0x2A)

typedef struct {
  unsigned long long from;
  unsigned long long to;
  unsigned long long offset;
  char flags[5];
  unsigned int minor;
  unsigned int major;
  unsigned int inode;
  char location[PATH_MAX];
} line_t;

typedef struct {
  line_t *lines;
  unsigned int nlines;
  unsigned long long length;
} pmap_t;

pmap_t create_pmap(pid_t pid);
void free_pmap(pmap_t *pmap);
