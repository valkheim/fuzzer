#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/personality.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <limits.h>

#include "fuzzer.h"
#include "ptrace.h"
#include "file.h"
#include "utils.h"
#include "snapshot.h"
#include "mutation.h"
#include "coverage.h"

static void debugee(fuzzer_t *fuzzer)
{
  // Request that the parent trace the child
  ptrace(PTRACE_TRACEME, 0, 0, 0);

  // disable ASLR
  personality(ADDR_NO_RANDOMIZE);


  LOG("%s", "enable tracee output\n");
#if 1
  int fd = open("/dev/null", O_WRONLY);
  dup2(fd, 1);
  dup2(fd, 2);
  close(fd);
#endif

  // Exec debugee program.
  // Replaces the fork() clone of the parent with the debugee process
  execl(
    fuzzer->target,
    fuzzer->target,
    fuzzer->input->filename,
    NULL
  );
}

static snapshot_t *init_tracee_state(fuzzer_t *fuzzer, pid_t child)
{
  int wait_status;
  struct user_regs_struct registers;

  wait(&wait_status);
  // set breakpoints at start/stop of target computing
  long long unsigned begin_value = get_value(child, fuzzer->boundaries.begin);
  set_breakpoint(fuzzer->boundaries.begin, begin_value, child);
  long long unsigned end_value = get_value(child, fuzzer->boundaries.end);
  set_breakpoint(fuzzer->boundaries.end, end_value, child);

  resume_execution(child);

  wait(&wait_status);
  if (WIFSTOPPED(wait_status))
  {
    if (WSTOPSIG(wait_status) == SIGTRAP)
    {
      registers = get_regs(child);
      LOG("reached breakpoint at 0X%llx\n", registers.rip - 1);
    } else {
      LOG("debugee signaled, reason : %s\n", strsignal(WSTOPSIG(wait_status)));
    }
  } else {
    ferr("ptrace", __FILE__, __LINE__);
    return NULL;
  }

  // remove start checkpoint
  unset_breakpoint(fuzzer->boundaries.begin, begin_value, child);

  // rewind RIP so we're ready to go
  registers.rip -= 1;
  set_regs(child, registers);

  // set code coverage checkpoints
  set_coverage_checkpoints(fuzzer, child);

  // snapshot tracee memory
  snapshot_t *snapshot = create_snapshot(child);
  return snapshot;
}

static void log_crash(mutation_t *mutation, int signum, struct user_regs_struct *registers)
{
  FILE *fptr;
  char path[PATH_MAX];

  sprintf(path, "crashes/%s.%llx", strsignal(signum), registers->rip - 1);
  fptr = fopen(path, "wb");
  fwrite(mutation->data, 1, mutation->length, fptr);
  fclose(fptr);
}

static int wait_fuzzcase(fuzzer_t *fuzzer, pid_t child, mutation_t *mutation)
{
  int wait_status;

  wait(&wait_status);
  if (WIFEXITED(wait_status))
    LOG("Exited normally. Status: %d\n", WEXITSTATUS(wait_status));

  if (WIFSTOPPED(wait_status))
  {
    struct user_regs_struct registers = get_regs(child);
    int signum = WSTOPSIG(wait_status);
    switch (signum) {
      case SIGTRAP:
        LOG("Fuzz case breaks on rip = 0x%llx\n", registers.rip - 1);
        if (registers.rip - 1 != fuzzer->boundaries.end)
          hit_checkpoint(fuzzer, child, &registers);

        break;
      case SIGSEGV:
      case SIGABRT:
        LOG("Fuzz case crashed on rip =  0x%llx\n", registers.rip - 1);
        fuzzer->crashes += 1;
        log_crash(mutation, signum, &registers);
        break;
      default:
        LOG("[-] run signaled: %s\n", strsignal(WSTOPSIG(wait_status)));
        break;
    }
  } else {
    ferr("wait", __FILE__, __LINE__);
    return 1;
  }

  return 0;
}

static void log_progress(fuzzer_t *fuzzer, clock_t start_at)
{
  if (fuzzer->iterations % 50000 != 0)
    return;

  clock_t step = clock() - start_at;
  double seconds_elapsed = (double)step / CLOCKS_PER_SEC;
  unsigned long long result = (unsigned long long) fuzzer->iterations / seconds_elapsed;
  double million_iterations = (double)fuzzer->iterations / 1000000;

  CLS();
  printf("execs:        %.1f millions\n", million_iterations);
  printf("execs:        %llu per second\n", result);
  printf("checkpoints:  %u/%u\n", fuzzer->corpus.count, CHECKPOINTS_LEN);
  printf("crashes:      %llu\n", fuzzer->crashes);
}

static void run_fuzzcases(fuzzer_t *fuzzer, snapshot_t *snapshot, pid_t child)
{
  clock_t start_at = clock();

  srand((unsigned)time(NULL));
  LOG("%s\n", "[+] Run fuzzcases");
  while (1)
  {
    // mutate input
    mutation_t mutation = create_mutation(fuzzer);

    // inject input into tracee memory
    inject_mutation(&mutation, child);

    // resume tracee execution
    resume_execution(child);

    // wait for breakpoint / crash / success
    wait_fuzzcase(fuzzer, child, &mutation);

    free(mutation.data);

    // reset tracee virtual memory using snapshot
    restore_snapshot(snapshot, child);

    fuzzer->iterations += 1;

    // log
    log_progress(fuzzer, start_at);
  }
}

static int debugger(fuzzer_t *fuzzer, pid_t child)
{
  printf("child pid: %d\n", child);
  snapshot_t *snapshot = init_tracee_state(fuzzer, child);
  run_fuzzcases(fuzzer, snapshot, child);

  return 0;
}

void trace(fuzzer_t *fuzzer)
{
  pid_t child = fork();
  if (child == 0)
  {
    debugee(fuzzer);
  } else {
    debugger(fuzzer, child);
  }
}

void free_fuzzer(fuzzer_t *fuzzer)
{
  for (unsigned int i = 0 ; i < fuzzer->corpus.count ; ++i)
    free(fuzzer->corpus.inputs[i]);

  free(fuzzer->corpus.inputs);
  free_file(fuzzer->input);
}
