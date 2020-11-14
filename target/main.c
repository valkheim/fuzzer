#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

typedef struct {
  char *data;
  size_t length;
} user_input_t;


user_input_t *read_input(char *filename)
{
  FILE *fptr;
  char *buffer;
  user_input_t *input;

  //printf("input=%d\n",filename);
  input = malloc(sizeof(*input));
  fptr = fopen(filename, "rb");
  fseek(fptr, 0, SEEK_END);
  input->length = ftell(fptr);
  //printf("len=%d\n", input->length);
  rewind(fptr);
  input->data = malloc(input->length * sizeof(*(input->data)));
  fread(input->data, input->length, 1, fptr);
  fclose(fptr);
  return input;
}

void print_input(user_input_t *input)
{
  write(1, input->data, input->length);
}

void check1(char *buffer, int check)
{
  if (buffer[check] != '\x6c')
  {
    puts("break on check1");
    exit(1);
  }
}

void check2(char *buffer, int check)
{
  if (buffer[check] != '\x57')
  {
    puts("break on check2");
    exit(2);
  }
}

void check3(char *buffer, int check)
{
  if (buffer[check] != '\x21')
  {
    puts("break on check3");
    exit(3);
  }
}

void vulnerable(char *buffer, size_t length)
{
  char vuln[(int)(length * .5)];
  memcpy(vuln, buffer, length);
}

int main(int ac, char **av)
{
  if (ac != 2)
  {
    fprintf(stderr, "Usage: vulnerable <file>\n");
    return -1;
  }

  // Read user input from file
  user_input_t *input = read_input(av[1]);

  // At this point, uesr input is loaded into heap memory
  print_input(input);

  // Simple control flow graph
  check1(input->data, input->length * .33);
  check2(input->data, input->length * .5);
  check3(input->data, input->length * .67);

  // Can you reach vulnerable code?
  vulnerable(input->data, input->length);
  exit(4);
}
