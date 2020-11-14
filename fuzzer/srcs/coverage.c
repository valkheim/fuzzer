#include <sys/types.h>
#include <sys/user.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "coverage.h"
#include "fuzzer.h"
#include "ptrace.h"
#include "utils.h"

void set_coverage_checkpoints(fuzzer_t *fuzzer, pid_t pid)
{
  for (unsigned int i = 0 ; i < fuzzer->ncheckpoints ; ++i)
  {
    fuzzer->checkpoints[i].original_value = get_value(pid, fuzzer->checkpoints[i].address);
    set_breakpoint(fuzzer->checkpoints[i].address, fuzzer->checkpoints[i].original_value, pid);
    LOG("checkpoint at 0x%llx (original value: 0x%llx\n",
        fuzzer->checkpoints[i].address,
        fuzzer->checkpoints[i].original_value
       );
  }
}

void unset_coverage_checkpoint(fuzzer_t *fuzzer, pid_t pid, long long unsigned checkpoint_address)
{
  for (unsigned int i = 0 ; i < fuzzer->ncheckpoints ; ++i)
  {
    if (fuzzer->checkpoints[i].address == checkpoint_address)
    {
      unset_breakpoint(checkpoint_address, fuzzer->checkpoints[i].original_value, pid);
    }
  }
}

void hit_checkpoint(fuzzer_t *fuzzer, pid_t pid, struct user_regs_struct *registers)
{
  unset_coverage_checkpoint(fuzzer, pid, registers->rip - 1);

  // save input that permitted checkpoint hit
  if (fuzzer->corpus.count + 1 >= CORPUS_MAX)
  {
    fprintf(stderr, "[-] Cannot add another %d corpus. Exiting...\n", fuzzer->corpus.count + 1);
    exit(1);
  }

  fuzzer->corpus.inputs[fuzzer->corpus.count] = malloc(fuzzer->input->size);
  memcpy(fuzzer->corpus.inputs[fuzzer->corpus.count], fuzzer->input->data, fuzzer->input->size);
  fuzzer->corpus.count += 1;
}

