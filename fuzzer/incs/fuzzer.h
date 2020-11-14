#pragma once

#include "file.h"

#define CORPUS_MAX      (100)
#define CHECKPOINTS_LEN (4)

typedef struct {
  unsigned long long begin; // snapshot start virtual address
  unsigned long long end;   // snapshot end virtual address
} boundaries_t;

typedef struct {
  unsigned long long address;         // breakpoint location
  unsigned long long original_value;  // original code under breakpoint 0xCC
} breakpoint_t;

typedef struct {
  unsigned int    count;  // number of mutations created so far
  unsigned char **inputs; // inputs responsible for cfg discovery
} corpus_t;

typedef struct {
  char                *target;
  file_t              *input;
  unsigned long long  iterations;
  unsigned long long  crashes;
  unsigned long long  input_location;
  boundaries_t        boundaries;
  unsigned int        ncheckpoints;
  corpus_t            corpus;
  breakpoint_t        checkpoints[CHECKPOINTS_LEN];
} fuzzer_t;

void trace(fuzzer_t *fuzzer);
void free_fuzzer(fuzzer_t *fuzzer);
