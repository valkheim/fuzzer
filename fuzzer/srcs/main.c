#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fuzzer.h"
#include "file.h"

static void get_fuzzer_config(fuzzer_t *fuzzer, char **av)
{
  fuzzer->target = av[1];
  fuzzer->input = read_file(av[2]);
  fuzzer->iterations = 0;
  fuzzer->crashes = 0;

  // when user input has been injected into tracee memory
  // so we can snapshot and mutate it
  fuzzer->boundaries.begin = 0x5555555552be;
  // when the fuzzer shoud discard fuzzcase
  fuzzer->boundaries.end = 0x00005555555550e0; // call exit

  // input location
  fuzzer->input_location = 0x55555555a4b0;

  // code coverage checkpoints
  breakpoint_t checkpoints[CHECKPOINTS_LEN] = {
    { 0x5555555552dd, 0 }, // check1
    { 0x555555555319, 0 }, // check2
    { 0x555555555355, 0 }, // check3
    { 0x555555555391, 0 }, // vulnerable
  };
  memcpy(fuzzer->checkpoints, checkpoints, sizeof(checkpoints));
  fuzzer->ncheckpoints = CHECKPOINTS_LEN;

  // init corpus of CORPUS_MAX inputs
  fuzzer->corpus.count = 0;
  fuzzer->corpus.inputs = malloc(CORPUS_MAX * sizeof(*fuzzer->corpus.inputs));
  for (unsigned int  i = 0 ; i < CORPUS_MAX ; ++i)
    fuzzer->corpus.inputs[i] = (unsigned char *)calloc(fuzzer->input->size, sizeof(*fuzzer->corpus.inputs));
}

static int usage(void)
{
  fprintf(stderr, "Usage: fuzzer <target> <input>");
  return 2;
}

int main(int ac, char **av)
{
  fuzzer_t fuzzer;

  if (ac < 3)
    return usage();

  get_fuzzer_config(&fuzzer, av);
  trace(&fuzzer);

  free_fuzzer(&fuzzer);
  return 0;
}
