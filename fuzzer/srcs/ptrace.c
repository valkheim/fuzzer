#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <errno.h>

#include "ptrace.h"
#include "utils.h"

long long unsigned get_value(pid_t pid, long long unsigned address)
{
  long value = ptrace(PTRACE_PEEKTEXT, pid, (void*)address, 0);
  if (value == -1 && errno != 0)
    ferrx("ptrace", __FILE__, __LINE__);

  return (long long unsigned)value;
}

void set_breakpoint(long long unsigned bp_addr, long long unsigned original_value, pid_t pid)
{
  unsigned long long bp_value = (original_value & ~0xff) | INSN_BREAK;
  if (ptrace(PTRACE_POKETEXT, pid, (void*)bp_addr, (void*)bp_value) == -1)
    ferrx("ptrace", __FILE__, __LINE__);
}

void unset_breakpoint(long long unsigned bp_addr, long long unsigned original_value, pid_t pid)
{
  if (ptrace(PTRACE_POKETEXT, pid, (void*)bp_addr, (void*)original_value) == -1)
    ferrx("ptrace", __FILE__, __LINE__);
}

void resume_execution(pid_t pid)
{
  if (ptrace(PTRACE_CONT, pid, 0, 0) == -1)
    ferrx("ptrace", __FILE__, __LINE__);
}

struct user_regs_struct get_regs(pid_t pid)
{
  struct user_regs_struct registers;
  if (ptrace(PTRACE_GETREGS, pid, 0, &registers) == -1)
    ferrx("ptrace", __FILE__, __LINE__);

  return registers;
}

void set_regs(pid_t pid, struct user_regs_struct registers)
{
  if (ptrace(PTRACE_SETREGS, pid, 0, &registers) == -1)
    ferrx("ptrace", __FILE__, __LINE__);
}
