#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "file.h"

file_t *read_file(char *filename)
{
  file_t *file;
  FILE *fptr;

  file = malloc(sizeof(*file));
  file->filename = strdup(filename);
  fptr = fopen(filename, "rb");
  if (fptr == NULL)
  {
    fprintf(stderr, "error %d during ", errno);
    perror("fopen");
    exit(errno);
  }
  fseek(fptr, 0, SEEK_END);
  file->size = (unsigned long long)ftell(fptr);
  rewind(fptr);

  file->data = malloc(file->size * sizeof(*file->data));
  fread(file->data, file->size, 1, fptr);
  fclose(fptr);
  return file;
}

void free_file(file_t *file)
{
  free(file->filename);
  free(file->data);
  free(file);
}
