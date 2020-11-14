#pragma once

typedef struct {
  char                *filename;
  unsigned char       *data;
  unsigned long long  size;
} file_t;

file_t *read_file(char *filename);
void free_file(file_t *file);
