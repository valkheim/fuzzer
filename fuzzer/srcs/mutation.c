#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#include <sys/uio.h>

#include "mutation.h"
#include "file.h"
#include "utils.h"
#include "mutators.h"

static inline void mutate(unsigned char *input, int length)
{
  mutator_rand_byte(input, length);
}

static unsigned char *create_mutation_from_initial_input(fuzzer_t *fuzzer)
{
  unsigned char *input = malloc(fuzzer->input->size * sizeof(*input));
  memcpy(input, fuzzer->input->data, fuzzer->input->size);

  mutate(input, fuzzer->input->size);

  return input;
}

static unsigned char *create_mutation_from_corpus(fuzzer_t *fuzzer)
{
  if (rand() % 5 == 0) // 5% chance to reset from initial input to avoit rabbit hole
    return create_mutation_from_initial_input(fuzzer);

  int rindex = rand() % fuzzer->corpus.count;
  unsigned char *input = malloc(fuzzer->input->size * sizeof(*input));
  memcpy(input, fuzzer->corpus.inputs[rindex], fuzzer->input->size);

  mutate(input, fuzzer->input->size);

  return input;
}

mutation_t create_mutation(fuzzer_t *fuzzer)
{
  mutation_t mutation = {
    NULL,
    fuzzer->input->size,
    fuzzer->input_location,
  };

  if (fuzzer->corpus.count == 0)
    mutation.data = create_mutation_from_initial_input(fuzzer);
  else
    mutation.data = create_mutation_from_corpus(fuzzer);

  return mutation;
}

ssize_t inject_mutation(mutation_t *mutation, pid_t pid)
{
  ssize_t written;
  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = mutation->data;
  local[0].iov_len = mutation->length;

  remote[0].iov_base = (void *)mutation->location;
  remote[0].iov_len = mutation->length;

  written = process_vm_writev(pid, local, 1, remote, 1, 0);
  LOG("[+] mutation injected: %lx bytes injected\n", written);
  return written;
}
