#pragma once

#include <sys/types.h>

#include "fuzzer.h"
#include "file.h"

typedef struct {
  unsigned char       *data;
  unsigned long long  length;
  unsigned long long  location; // address in fuzzcase virtual memory
} mutation_t;

mutation_t create_mutation(fuzzer_t *fuzzer);
ssize_t inject_mutation(mutation_t *mutation, pid_t pid);
