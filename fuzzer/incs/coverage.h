#pragma once

#include <sys/types.h>

#include "fuzzer.h"

void set_coverage_checkpoints(fuzzer_t *fuzzer, pid_t pid);
void unset_coverage_checkpoint(fuzzer_t *fuzzer, pid_t pid, long long unsigned checkpoint_address);
void hit_checkpoint(fuzzer_t *fuzzer, pid_t pid, struct user_regs_struct *registers);
